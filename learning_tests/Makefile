CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

TARGETS = \
	01_thread_lifecycle \
	02_mutex_counter \
	03_dongle_locking \
	04_monitor_and_scheduler

all: $(TARGETS)

01_thread_lifecycle: 01_thread_lifecycle.c
	$(CC) $(CFLAGS) $< -o $@

02_mutex_counter: 02_mutex_counter.c
	$(CC) $(CFLAGS) $< -o $@

03_dongle_locking: 03_dongle_locking.c
	$(CC) $(CFLAGS) $< -o $@

04_monitor_and_scheduler: 04_monitor_and_scheduler.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
