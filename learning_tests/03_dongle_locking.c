#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/* Number of coders and dongles (one dongle between each pair). */
#define CODERS 5
/* How many compile cycles each coder performs in this mini simulation. */
#define ROUNDS 3
/* Timing values (in seconds) to make logs easier to read by humans. */
#define THINK_SEC 3
#define FIRST_TO_SECOND_LOCK_DELAY_SEC 1
#define UNORDERED_EXTRA_STAGGER_SEC 2
#define COMPILE_SEC 5

/*
** Global simulation state shared by all coder threads.
** - dongles[]: one mutex per dongle (resource lock)
** - print_lock: serialize stdout logs (avoid interleaved lines)
** - ordered_locking:
**   0 -> lock left then right
**   1 -> lock lower index then higher index (global order)
*/
typedef struct s_sim
{
	pthread_mutex_t	dongles[CODERS];
	pthread_mutex_t	print_lock;
	int			ordered_locking;
} t_sim;

/* Per-thread argument package. */
typedef struct s_coder
{
	int	id;
	t_sim	*sim;
} t_coder;

/*
** Print one log line while holding print mutex.
** This prevents two threads from mixing characters on one line.
*/
static void	log_state(t_sim *sim, int id, const char *msg)
{
	pthread_mutex_lock(&sim->print_lock);
	printf("coder %d %s\n", id, msg);
	pthread_mutex_unlock(&sim->print_lock);
}

/*
** Lock the two dongles needed by one coder.
**
** left  = coder id
** right = next index in circular table
**
** Strategy A (ordered_locking=0): left then right.
** Strategy B (ordered_locking=1): min index then max index.
** Strategy B is the classic deadlock-avoidance approach because all
** threads follow a single global ordering rule.
*/
static void	lock_dongles(t_coder *coder)
{
	int	left;
	int	right;
	int	first;
	int	second;

	left = coder->id;
	right = (coder->id + 1) % CODERS;
	if (coder->sim->ordered_locking)
	{
		first = left < right ? left : right;
		second = left < right ? right : left;
	}
	else
	{
		first = left;
		second = right;
	}
	/* Lock first dongle, then second dongle. */
	pthread_mutex_lock(&coder->sim->dongles[first]);
	log_state(coder->sim, coder->id, "has taken first dongle");
	/* Delay to make the timeline readable in terminal output. */
	sleep(FIRST_TO_SECOND_LOCK_DELAY_SEC);
	pthread_mutex_lock(&coder->sim->dongles[second]);
	log_state(coder->sim, coder->id, "has taken second dongle");
}

/*
** Release this coder's left and right dongles.
** Important: lock/unlock pairings must be consistent and complete.
*/
static void	unlock_dongles(t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id;
	right = (coder->id + 1) % CODERS;
	pthread_mutex_unlock(&coder->sim->dongles[left]);
	pthread_mutex_unlock(&coder->sim->dongles[right]);
}

/*
** Main routine for each coder thread.
** Loop = thinking -> lock two dongles -> compiling -> unlock.
*/
static void	*coder_routine(void *arg)
{
	t_coder	*coder;
	int		i;

	/* Recover typed data from generic pthread argument. */
	coder = (t_coder *)arg;
	i = 0;
	while (i < ROUNDS)
	{
		log_state(coder->sim, coder->id, "is thinking");
		sleep(THINK_SEC);
		/*
		** Extra stagger for unordered mode so demo progresses more visibly.
		** Remove this line if you want a riskier locking pattern.
		*/
		if (!coder->sim->ordered_locking && (coder->id % 2 == 0))
			sleep(UNORDERED_EXTRA_STAGGER_SEC);
		lock_dongles(coder);
		log_state(coder->sim, coder->id, "is compiling");
		sleep(COMPILE_SEC);
		unlock_dongles(coder);
		log_state(coder->sim, coder->id, "released dongles");
		i++;
	}
	return (NULL);
}

/*
** Build one simulation run with chosen locking strategy.
** Creates/destroys all mutexes and threads for a clean experiment.
*/
static void	run_demo(int ordered_locking)
{
	t_sim		sim;
	t_coder		coders[CODERS];
	/*
	** Thread handle array (one handle per coder).
	** Not a function call: plain array declaration.
	*/
	pthread_t	threads[CODERS];
	int			i;

	sim.ordered_locking = ordered_locking;
	/* Mutex protecting logs. */
	pthread_mutex_init(&sim.print_lock, NULL);
	/* One mutex per dongle resource. */
	i = 0;
	while (i < CODERS)
	{
		pthread_mutex_init(&sim.dongles[i], NULL);
		i++;
	}
	printf("\n--- ordered_locking=%d ---\n", ordered_locking);
	i = 0;
	while (i < CODERS)
	{
		/* Prepare thread args for coder i. */
		coders[i].id = i;
		coders[i].sim = &sim;
		/* Launch one thread per coder. */
		pthread_create(&threads[i], NULL, coder_routine, &coders[i]);
		i++;
	}
	/* Wait for all coders to finish. */
	i = 0;
	while (i < CODERS)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	/* Destroy all dongle mutexes. */
	i = 0;
	while (i < CODERS)
	{
		pthread_mutex_destroy(&sim.dongles[i]);
		i++;
	}
	/* Destroy log mutex. */
	pthread_mutex_destroy(&sim.print_lock);
}

int	main(void)
{
	/* Compare both strategies in one execution. */
	printf("Try changing ROUNDS/CODERS and lock order strategy.\n");
	run_demo(0);
	run_demo(1);
	return (0);
}
