/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 15:45:54 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 15:52:52 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Initializes the simulation: records start time, allocates coder
 * array, spawns all coder threads plus the burnout monitor, waits
 * for all threads to finish, then prints the outcome.
 */
int	start_simulation(t_sim *sim)
{
	int	i;

	sim->start_time = get_time();
	sim->is_running = 1;
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!sim->coders)
		return (1);
	init_coders(sim);
	pthread_create(&sim->burnout_thread, NULL, burnout_monitor, sim);
	i = -1;
	while ((unsigned int)++i < sim->nb_coders)
		pthread_join(sim->coders[i].thread, NULL);
	sim_ended(sim);
	pthread_join(sim->burnout_thread, NULL);
	return (0);
}

/*
 * Thread-safe read of sim->is_running under sim_lock.
 */
int	is_simulation_running(t_sim *sim)
{
	int	running;

	pthread_mutex_lock(&sim->sim_lock);
	running = sim->is_running;
	pthread_mutex_unlock(&sim->sim_lock);
	return (running);
}

/*
 * Stops the simulation: sets is_running = 0, prints the burnout
 * message under print_lock, then broadcasts all dongle conditions
 * so waiting threads can exit cleanly.
 */
void	end_simulation(t_sim *sim, int coder_id, int has_printed)
{
	size_t	current_time;

	pthread_mutex_lock(&sim->sim_lock);
	sim->is_running = 0;
	pthread_mutex_unlock(&sim->sim_lock);
	current_time = get_time();
	pthread_mutex_lock(&sim->print_lock);
	if (!has_printed)
		printf("\e[0;31m%-5zu %-5d has burned out\e[0;37m\n",
			current_time - sim->start_time, sim->coders[coder_id].id + 1);
	pthread_mutex_unlock(&sim->print_lock);
	stop_and_broadcast(sim);
}

/*
 * Destroys all coder mutexes, dongle mutexes, dongle condition
 * variables, and the simulation-level mutexes.
 */
void	cleanup_sim(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < (int)sim->nb_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].coder_lock);
		pthread_mutex_destroy(&sim->dongles[i].dongle_lock);
		pthread_cond_destroy(&sim->dongles[i].dongle_cond);
	}
	pthread_mutex_destroy(&sim->print_lock);
	pthread_mutex_destroy(&sim->sim_lock);
}
