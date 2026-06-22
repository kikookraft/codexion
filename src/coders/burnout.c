/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   burnout.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 12:32:36 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/22 15:28:49 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Iterates all coders and returns the index of the first one
 * that has burned out, or -1 if none have.
 */
static int	check_burnouts(t_sim *sim, size_t current_time)
{
	int	i;

	(void)current_time;
	i = -1;
	while (++i < (int)sim->nb_coders)
	{
		if (has_coder_burned_out(&sim->coders[i]))
			return (i);
	}
	return (-1);
}

/*
 * Monitor thread: polls every 1 ms under sim_lock. If a coder
 * has burned out, calls end_simulation to stop the simulation.
 * The burnout message is printed later in start_simulation
 * after all coder threads have joined, preventing log races.
 */
void	*burnout_monitor(void *arg)
{
	t_sim	*sim;
	int		burnout_coder;

	sim = (t_sim *)arg;
	while (1)
	{
		safe_mutex_lock(&sim->sim_lock);
		if (!sim->is_running)
		{
			safe_mutex_unlock(&sim->sim_lock);
			break ;
		}
		burnout_coder = check_burnouts(sim, get_time());
		safe_mutex_unlock(&sim->sim_lock);
		if (burnout_coder >= 0)
		{
			end_simulation(sim, burnout_coder);
			break ;
		}
		usleep(1000);
	}
	return (NULL);
}

/*
 * Sets is_running = 0 under sim_lock, then broadcasts on every
 * dongle condition variable to wake all blocked coder threads.
 */
void	stop_and_broadcast(t_sim *sim)
{
	int	i;

	safe_mutex_lock(&sim->sim_lock);
	sim->is_running = 0;
	safe_mutex_unlock(&sim->sim_lock);
	i = -1;
	while (++i < (int)sim->nb_coders)
	{
		safe_mutex_lock(&sim->dongles[i].dongle_lock);
		pthread_cond_broadcast(&sim->dongles[i].dongle_cond);
		safe_mutex_unlock(&sim->dongles[i].dongle_lock);
	}
}

/*
 * Thread-safe burnout check: returns 1 if the coder has not started
 * compiling within burnout_time ms since its last compile start.
 */
int	has_coder_burned_out(t_coder *coder)
{
	safe_mutex_lock(&coder->coder_lock);
	if (get_time() - coder->last_compile_start >= coder->sim->burnout_time)
	{
		safe_mutex_unlock(&coder->coder_lock);
		return (1);
	}
	safe_mutex_unlock(&coder->coder_lock);
	return (0);
}
