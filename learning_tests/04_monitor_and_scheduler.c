#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/* Number of coder threads in this sandbox simulation. */
#define CODERS 4
/* Stop normally once every coder reached this compile count. */
#define TARGET_COMPILES 3
/* If a coder waits more than this since last compile start -> burnout. */
#define TIME_TO_BURNOUT_MS 80
/* Duration of each state (milliseconds). */
#define TIME_TO_COMPILE_MS 20
#define TIME_TO_DEBUG_MS 15
#define TIME_TO_REFACTOR_MS 15

/* Scheduler mode used when inserting requests into arbitration queue. */
typedef enum e_scheduler
{
	MODE_FIFO,
	MODE_EDF
} t_scheduler;

/* Forward declaration because t_thread_arg stores t_sim*. */
typedef struct s_sim	t_sim;

/* Per-thread argument package for coder thread creation. */
typedef struct s_thread_arg
{
	t_sim	*sim;
	int	coder_id;
} t_thread_arg;

/*
** Node used in waiting queue:
** - coder_id: which coder requested dongles
** - deadline_ms: used by EDF mode ordering
** - seq: monotonic insertion number to keep stable ordering on ties
*/
typedef struct s_request
{
	int			coder_id;
	long		deadline_ms;
	long		seq;
	struct s_request	*next;
} t_request;

/* Runtime state for one coder. */
typedef struct s_coder
{
	int	id;
	int	compiles_done;
	long	last_compile_start_ms;
} t_coder;

/*
** Global shared simulation state.
** lock: protects queue/stop/coder counters/dongle availability.
** log_lock: serializes stdout lines.
** cond: wakes waiting coders when queue or availability changes.
*/
typedef struct s_sim
{
	pthread_mutex_t	lock;
	pthread_mutex_t	log_lock;
	pthread_cond_t	cond;
	t_request	*queue_head;
	long		next_seq;
	int		dongle_pair_busy;
	int		stop;
	t_scheduler	scheduler;
	long		start_ms;
	t_coder		coders[CODERS];
} t_sim;

/* Current time in milliseconds since Unix epoch. */
static long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000L));
}

/*
** Thread-safe log printer.
** Output format matches project-style timestamp + coder id + state message.
*/
static void	log_msg(t_sim *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->log_lock);
	printf("%ld %d %s\n", now_ms() - sim->start_ms, coder_id, msg);
	pthread_mutex_unlock(&sim->log_lock);
}

/*
** Insert request in queue:
** - FIFO: append at end
** - EDF: keep list sorted by earliest deadline, then by sequence number
*/
static void	queue_push(t_sim *sim, t_request *node)
{
	t_request	**it;

	it = &sim->queue_head;
	if (sim->scheduler == MODE_FIFO)
	{
		while (*it)
			it = &(*it)->next;
	}
	else
	{
		while (*it)
		{
			if ((*it)->deadline_ms > node->deadline_ms)
				break ;
			if ((*it)->deadline_ms == node->deadline_ms && (*it)->seq > node->seq)
				break ;
			it = &(*it)->next;
		}
	}
	node->next = *it;
	*it = node;
}

/* Remove specific node if still present (used on shutdown race). */
static void	queue_remove(t_sim *sim, t_request *node)
{
	t_request	**it;

	it = &sim->queue_head;
	while (*it)
	{
		if (*it == node)
		{
			*it = node->next;
			node->next = NULL;
			return ;
		}
		it = &(*it)->next;
	}
}

/* Remove current queue head after it has been granted access. */
static void	queue_pop_head(t_sim *sim)
{
	t_request	*head;

	head = sim->queue_head;
	if (!head)
		return ;
	sim->queue_head = head->next;
	head->next = NULL;
}

/*
** Coder asks for dongles (modeled as one shared pair in this sandbox).
** Steps:
** 1) push request in queue
** 2) wait until request is queue head AND resource is free
** 3) mark resource busy + refresh compile-start timestamp
*/
static void	request_dongles(t_sim *sim, int coder_id)
{
	t_request	req;
	long		deadline;

	pthread_mutex_lock(&sim->lock);
	deadline = sim->coders[coder_id].last_compile_start_ms + TIME_TO_BURNOUT_MS;
	req.coder_id = coder_id;
	req.deadline_ms = deadline;
	req.seq = sim->next_seq++;
	req.next = NULL;
	queue_push(sim, &req);
	/* Wait while not granted and simulation not stopped. */
	while (!sim->stop && (sim->queue_head != &req || sim->dongle_pair_busy))
		pthread_cond_wait(&sim->cond, &sim->lock);
	if (!sim->stop)
	{
		/* This request now owns the resource. */
		queue_pop_head(sim);
		sim->dongle_pair_busy = 1;
		sim->coders[coder_id].last_compile_start_ms = now_ms();
	}
	else
		/* Stop happened while waiting: remove stale request if queued. */
		queue_remove(sim, &req);
	pthread_mutex_unlock(&sim->lock);
}

/* Release resource and wake all waiters to re-check queue conditions. */
static void	release_dongles(t_sim *sim)
{
	pthread_mutex_lock(&sim->lock);
	sim->dongle_pair_busy = 0;
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->lock);
}

/*
** Coder thread routine:
** loop until stop or target reached, then do compile/debug/refactor cycle.
*/
static void	*coder_thread(void *arg)
{
	t_thread_arg	*thread_arg;
	t_sim			*sim;
	int				id;

	thread_arg = (t_thread_arg *)arg;
	sim = thread_arg->sim;
	id = thread_arg->coder_id;
	while (1)
	{
		/* Fast stop/goal check under lock. */
		pthread_mutex_lock(&sim->lock);
		if (sim->stop || sim->coders[id].compiles_done >= TARGET_COMPILES)
		{
			pthread_mutex_unlock(&sim->lock);
			break ;
		}
		pthread_mutex_unlock(&sim->lock);
		/* Block until scheduler grants resource. */
		request_dongles(sim, id);
		pthread_mutex_lock(&sim->lock);
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->lock);
			break ;
		}
		/* Count this compile cycle. */
		sim->coders[id].compiles_done++;
		pthread_mutex_unlock(&sim->lock);
		/* Subject-like logs for each state transition. */
		log_msg(sim, id + 1, "has taken a dongle");
		log_msg(sim, id + 1, "has taken a dongle");
		log_msg(sim, id + 1, "is compiling");
		usleep(TIME_TO_COMPILE_MS * 1000);
		release_dongles(sim);
		log_msg(sim, id + 1, "is debugging");
		usleep(TIME_TO_DEBUG_MS * 1000);
		log_msg(sim, id + 1, "is refactoring");
		usleep(TIME_TO_REFACTOR_MS * 1000);
	}
	return (NULL);
}

/*
** Monitor thread:
** - polls frequently for burnout precision
** - sets stop when burnout occurs or everyone finished target
*/
static void	*monitor_thread(void *arg)
{
	t_sim	*sim;
	int		all_done;
	int		i;
	long		now;

	sim = (t_sim *)arg;
	while (1)
	{
		/* 1ms poll interval: simple but precise enough for this demo. */
		usleep(1 * 1000);
		pthread_mutex_lock(&sim->lock);
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->lock);
			return (NULL);
		}
		all_done = 1;
		now = now_ms();
		i = 0;
		while (i < CODERS)
		{
			if (sim->coders[i].compiles_done < TARGET_COMPILES)
				all_done = 0;
			/* Burnout check: too long since last compile start. */
			if (sim->coders[i].compiles_done < TARGET_COMPILES
				&& (now - sim->coders[i].last_compile_start_ms) > TIME_TO_BURNOUT_MS)
			{
				/* Stop simulation and wake blocked coders. */
				sim->stop = 1;
				pthread_cond_broadcast(&sim->cond);
				pthread_mutex_unlock(&sim->lock);
				/* Print burnout event outside state lock. */
				log_msg(sim, i + 1, "burned out");
				return (NULL);
			}
			i++;
		}
		if (all_done)
		{
			/* Clean normal termination condition. */
			sim->stop = 1;
			pthread_cond_broadcast(&sim->cond);
			pthread_mutex_unlock(&sim->lock);
			return (NULL);
		}
		pthread_mutex_unlock(&sim->lock);
	}
}

/*
** Entry point:
** argv[1] optional: "fifo" or "edf".
** Launches CODERS worker threads + one monitor thread.
*/
int	main(int argc, char **argv)
{
	t_sim		sim;
	/* Arrays below are declarations, not function calls. */
	pthread_t	coder_threads[CODERS];
	t_thread_arg	thread_args[CODERS];
	pthread_t	monitor;
	int			i;

	/* Zero all fields (queue pointers, counters, flags...). */
	memset(&sim, 0, sizeof(sim));
	sim.scheduler = MODE_FIFO;
	/* Select scheduler mode from CLI if provided. */
	if (argc == 2 && strcmp(argv[1], "edf") == 0)
		sim.scheduler = MODE_EDF;
	else if (argc == 2 && strcmp(argv[1], "fifo") == 0)
		sim.scheduler = MODE_FIFO;
	/* Per-coder initial state. */
	i = 0;
	while (i < CODERS)
	{
		sim.coders[i].id = i;
		sim.coders[i].last_compile_start_ms = now_ms();
		i++;
	}
	/* Simulation start timestamp for relative logging. */
	sim.start_ms = now_ms();
	/* Initialize synchronization primitives. */
	pthread_mutex_init(&sim.lock, NULL);
	pthread_mutex_init(&sim.log_lock, NULL);
	pthread_cond_init(&sim.cond, NULL);
	/* Launch coder threads. */
	i = 0;
	while (i < CODERS)
	{
		thread_args[i].sim = &sim;
		thread_args[i].coder_id = i;
		pthread_create(&coder_threads[i], NULL, coder_thread, &thread_args[i]);
		i++;
	}
	/* Launch monitor thread. */
	pthread_create(&monitor, NULL, monitor_thread, &sim);
	/* Join all coder threads. */
	i = 0;
	while (i < CODERS)
	{
		pthread_join(coder_threads[i], NULL);
		i++;
	}
	/* Join monitor thread and cleanup. */
	pthread_join(monitor, NULL);
	pthread_cond_destroy(&sim.cond);
	pthread_mutex_destroy(&sim.log_lock);
	pthread_mutex_destroy(&sim.lock);
	printf("Simulation ended with scheduler=%s\n",
		sim.scheduler == MODE_FIFO ? "fifo" : "edf");
	return (0);
}
