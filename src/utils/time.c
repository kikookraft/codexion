/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:57:17 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 17:13:43 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"
/*
 * Returns a pointer to the hidden start time.
 */
static size_t	*get_start_time_ptr(void)
{
	static size_t	start_time;

	return (&start_time);
}

/*
 * Set the simulation start time.
 */
void	set_simulation_start_time(size_t start)
{
	*get_start_time_ptr() = start;
}

/*
 * Returns the number of milliseconds elapsed since the
 * simulation began. No need to pass sim.
 */
size_t	get_elapsed_time(void)
{
	return (get_time() - *get_start_time_ptr());
}

/*
 * Returns the current wall-clock time in milliseconds
 * using gettimeofday.
 */
size_t	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((size_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

/*
 * Hybrid sleep: bulk-sleeps via usleep for (time - time / 2) ms, then
 * busy-waits the last remaining ms with 200 us polling. Guarantees at
 * least 'time' ms of sleep, preventing cycle drift.
 */
void	msleep(size_t time)
{
	size_t	start;

	if (time == 0)
		return ;
	start = get_time();
	if (time > (size_t)(time / 2))
		usleep((time - (size_t)(time / 2)) * 1000);
	while ((get_time() - start) < time)
		usleep(100);
}
