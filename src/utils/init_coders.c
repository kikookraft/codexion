/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 15:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/22 15:28:49 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Cleans up coder threads and mutexes on init failure.
 */
static void	cleanup_coders(t_sim *sim, unsigned int count)
{
	while (count > 0)
	{
		count--;
		pthread_join(sim->coders[count].thread, NULL);
		pthread_mutex_destroy(&sim->coders[count].coder_lock);
	}
}

/*
 * Initializes coder metadata (id, counters, timestamps) and
 * creates a thread for each coder running coder_routine.
 * On thread creation failure, cleans up previously created threads.
 * Returns 0 on success, 1 on failure.
 */
int	init_coders(t_sim *sim)
{
	unsigned int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->coders[i].id = i;
		sim->coders[i].times_compiled = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].sim = sim;
		sim->coders[i].is_waiting = 0;
		if (safe_mutex_init(&sim->coders[i].coder_lock)
			|| safe_thread_create(&sim->coders[i].thread,
				coder_routine, &sim->coders[i]))
		{
			cleanup_coders(sim, i);
			return (1);
		}
		i++;
	}
	return (0);
}
