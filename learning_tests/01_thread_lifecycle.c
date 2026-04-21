#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/* Number of threads we want to create in this demo. */
#define THREAD_COUNT 3
/* Number of loop iterations each worker thread will print. */
#define PRINT_ROUNDS 5

/*
** Small data package given to each thread.
** Each worker gets:
** - id: human-readable identifier (1, 2, 3...)
** - rounds: how many times it runs its loop
*/
typedef struct s_worker
{
	int id;
	int rounds;
} t_worker;

/*
** Thread entry function.
** pthread_create expects a function with this exact signature:
**   void *function_name(void *arg)
**
** - arg is one generic pointer passed by pthread_create.
** - return value is also a generic pointer (unused in this demo -> NULL).
*/
static void	*worker_run(void *arg)
{
	t_worker	*w;
	int		i;

	/* Convert generic void* to t_worker* so we can read fields. */
	w = (t_worker *)arg;
	i = 0;
	while (i < w->rounds)
	{
		/* One print per round. Multiple threads can interleave here. */
		printf("[worker %d] round %d\n", w->id, i + 1);
		/* Sleep 100ms to make output easier to observe. */
		usleep(1000 * 1000);
		i++;
	}
	/* Thread finished; no return data needed. */
	return (NULL);
}

/*
** Demo 1: create several threads and wait for all of them with join.
*/
static void	demo_create_and_join(void)
{
	/*
	** Array of thread handles (IDs managed by pthread library).
	** This is NOT a function call.
	**
	** pthread_t threads[THREAD_COUNT];
	** means: "create an array named threads with THREAD_COUNT elements".
	** Here THREAD_COUNT = 3, so we have threads[0], threads[1], threads[2].
	*/
	pthread_t	threads[THREAD_COUNT];
	/*
	** Array of worker structs, one struct per thread.
	** Also NOT a function call.
	**
	** t_worker workers[THREAD_COUNT];
	** means: "create an array of 3 t_worker structs".
	** We fill workers[i], then pass &workers[i] to thread i.
	*/
	t_worker	workers[THREAD_COUNT];
	int			i;

	printf("\n--- demo_create_and_join ---\n");
	i = 0;
	while (i < THREAD_COUNT)
	{
		/* Prepare arguments for thread i. */
		workers[i].id = i + 1;
		workers[i].rounds = PRINT_ROUNDS;
		/*
		** Create one thread:
		** - &threads[i] : where pthread stores thread handle
		** - NULL        : default thread attributes
		** - worker_run  : function the new thread will execute
		** - &workers[i] : pointer given as void *arg to worker_run
		*/
		pthread_create(&threads[i], NULL, worker_run, &workers[i]);
		i++;
	}
	i = 0;
	while (i < THREAD_COUNT)
	{
		/*
		** Wait for thread i to finish.
		** NULL means we ignore the return pointer from worker_run.
		*/
		pthread_join(threads[i], NULL);
		i++;
	}
	printf("All joined threads finished.\n");
}

/*
** Thread entry for detached-thread demo.
*/
static void	*detached_run(void *arg)
{
	/* Unused argument in this function. */
	(void)arg;
	printf("[detached] started\n");
	usleep(300 * 1000);
	printf("[detached] finished (no join needed)\n");
	return (NULL);
}

/*
** Demo 2: detached thread.
** A detached thread releases its resources automatically at end,
** so main thread must NOT call pthread_join on it.
*/
static void	demo_detached_thread(void)
{
	pthread_t	th;

	printf("\n--- demo_detached_thread ---\n");
	/* Create one thread that runs detached_run. */
	pthread_create(&th, NULL, detached_run, NULL);
	/* Mark it detached: no join needed/allowed later. */
	pthread_detach(th);
	/* Keep process alive long enough to observe detached output. */
	usleep(500 * 1000);
}

int	main(void)
{
	/* Run both independent demonstrations. */
	demo_create_and_join();
	demo_detached_thread();
	return (0);
}
