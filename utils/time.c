#include "inc.h"

size_t get_time(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((size_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

void msleep(size_t time)
{
	size_t start;
	start = get_time();
	while ((get_time() - start) < time)
		usleep(200);
}
