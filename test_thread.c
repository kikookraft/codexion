


void* foo(void* arg) {
	int i;

	(void)arg;

	i = -1;
	pthread_t thisThread = pthread_self();
	printf("New thread, ID: %lu\n", (unsigned long)thisThread);
	while (1)
	{
		i++;
		printf("waiting %ds...\n", i);
		sleep(1);
	}
    return NULL;
}


int	tempo(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	pthread_t thread1;
	pthread_t thread2;
    pthread_create(&thread1, NULL, foo, NULL);
	sleep(2);
    pthread_create(&thread2, NULL, foo, NULL);
	sleep(5);
	pthread_cancel(thread1);
	pthread_cancel(thread2);
    // Wait for thread to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
	printf("Thread finished\n");
    return 0;
}