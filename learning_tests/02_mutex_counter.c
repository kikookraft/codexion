#include <pthread.h>
#include <stdio.h>

/* Number of worker threads incrementing the same shared counter. */
#define THREADS 4
/* How many increments each thread performs. */
#define INCREMENTS 200000

/*
** Shared context used by all threads in this test.
** - value: shared counter (critical data)
** - lock: mutex used to protect value
** - use_lock: toggle (0=no mutex, 1=mutex)
*/
typedef struct s_counter_ctx
{
	long		value;
	pthread_mutex_t	lock;
	int		use_lock;
} t_counter_ctx;

/*
** Thread function: each thread increments ctx->value INCREMENTS times.
** When use_lock is 0, updates race each other (lost increments likely).
** When use_lock is 1, updates are serialized and final value is correct.
*/
static void	*counter_worker(void *arg)
{
	t_counter_ctx	*ctx;
	int			i;

	/* Recover typed pointer from generic void*. */
	ctx = (t_counter_ctx *)arg;
	i = 0;
	while (i < INCREMENTS)
	{
		/* Lock before entering critical section (if enabled). */
		if (ctx->use_lock)
			pthread_mutex_lock(&ctx->lock);
		/* Critical section: read-modify-write shared value. */
		ctx->value++;
		/* Unlock after leaving critical section (if enabled). */
		if (ctx->use_lock)
			pthread_mutex_unlock(&ctx->lock);
		i++;
	}
	return (NULL);
}

/*
** Launch THREADS workers, wait for all of them, print observed result.
** Parameter use_lock:
** - 0: race condition demo
** - 1: mutex-protected demo
*/
static void	run_test(int use_lock)
{
	/*
	** Array of thread handles.
	** Not a function call: this is a variable declaration.
	** threads[0..THREADS-1] store IDs used by pthread_join.
	*/
	pthread_t	threads[THREADS];
	/* One shared context instance, passed to all threads. */
	t_counter_ctx	ctx;
	int			i;
	long			expected;

	/* Initialize shared context. */
	ctx.value = 0;
	ctx.use_lock = use_lock;
	/* Create mutex with default attributes. */
	pthread_mutex_init(&ctx.lock, NULL);
	/* Start all worker threads. */
	i = 0;
	while (i < THREADS)
	{
		/*
		** pthread_create args:
		** - &threads[i]   output handle
		** - NULL          default thread attributes
		** - counter_worker function to execute
		** - &ctx          argument passed to the function
		*/
		pthread_create(&threads[i], NULL, counter_worker, &ctx);
		i++;
	}
	/* Wait for all workers to finish. */
	i = 0;
	while (i < THREADS)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	/* Mathematical expected value with no race: THREADS * INCREMENTS. */
	expected = (long)THREADS * INCREMENTS;
	printf("use_lock=%d -> counter=%ld (expected=%ld)\n",
		use_lock, ctx.value, expected);
	/* Always destroy mutex after last use. */
	pthread_mutex_destroy(&ctx.lock);
}

int	main(void)
{
	/* First run intentionally without lock to show race condition. */
	printf("Without mutex (race expected):\n");
	run_test(0);
	/* Second run with lock to show correct synchronization. */
	printf("With mutex (correct result expected):\n");
	run_test(1);
	return (0);
}
