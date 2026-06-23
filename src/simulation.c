/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 15:45:54 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/23 18:39:49 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Joins all coder threads (used both in normal completion
 * and during error cleanup).
 */
static void	join_coders(t_sim *sim)
{
	unsigned int	i;

	i = 0;
	while (i < sim->nb_coders)
		pthread_join(sim->coders[i++].thread, NULL);
}

/*
 * Initializes the simulation: records start time, allocates coder
 * array, spawns all coder threads plus the burnout monitor, waits
 * for all threads to finish, then returns.
 * Returns 0 on success, 1 on failure.
 */
int	start_simulation(t_sim *sim)
{
	sim->start_time = get_time();
	sim->is_running = 1;
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!sim->coders)
		return (1);
	if (init_coders(sim))
	{
		free(sim->coders);
		sim->coders = NULL;
		return (1);
	}
	if (safe_thread_create(&sim->burnout_thread, burnout_monitor, sim))
	{
		join_coders(sim);
		free(sim->coders);
		sim->coders = NULL;
		return (1);
	}
	join_coders(sim);
	safe_mutex_lock(&sim->sim_lock);
	sim->is_running = 0;
	safe_mutex_unlock(&sim->sim_lock);
	pthread_join(sim->burnout_thread, NULL);
	return (0);
}

/*
 * Thread-safe read of sim->is_running under sim_lock.
 */
int	is_simulation_running(t_sim *sim)
{
	int	running;

	safe_mutex_lock(&sim->sim_lock);
	running = sim->is_running;
	safe_mutex_unlock(&sim->sim_lock);
	return (running);
}

/*
 * Destroys all coder mutexes, dongle mutexes, dongle condition
 * variables, and the simulation-level mutexes.
 */
void	cleanup_sim(t_sim *sim)
{
	int	i;

	i = -1;
	if (sim->coders)
	{
		while (++i < (int)sim->nb_coders)
			pthread_mutex_destroy(&sim->coders[i].coder_lock);
	}
	i = -1;
	while (++i < (int)sim->nb_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].dongle_lock);
		pthread_cond_destroy(&sim->dongles[i].dongle_cond);
	}
	pthread_mutex_destroy(&sim->print_lock);
	pthread_mutex_destroy(&sim->sim_lock);
}
