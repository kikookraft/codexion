/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 12:05:39 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 17:09:00 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Returns a pointer to a static mutex pointer.
 */
static pthread_mutex_t **get_print_lock_ptr(void)
{
    static pthread_mutex_t *lock;

    return (&lock);
}

/*
 * One‑time setup: call right after pthread_mutex_init.
 */
void	print_set_mutex(pthread_mutex_t *lock)
{
	*get_print_lock_ptr() = lock;
}

/*
 * Thread-safe print_lock wrapper: locks if state is 1, unlocks if 0.
 * Does nothing if the mutex pointer is NULL.
*/
void	print_lock(int state)
{
	pthread_mutex_t	*lock;

	lock = *get_print_lock_ptr();
	if (lock)
	{
		if (state)
			pthread_mutex_lock(lock);
		else
			pthread_mutex_unlock(lock);
	}
}

/*
 * Print a message with timestamp and coder_id under print_lock.
 * Automatucly lock / unlock the print mutex.
 */
void	log_action(char *message, int coder_id)
{
	print_lock(1);
	printf("%-5zu %-5d %s\n", get_elapsed_time(), coder_id + 1, message);
	print_lock(0);
}
