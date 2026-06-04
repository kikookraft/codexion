/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   burnout.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 12:32:36 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/04 12:16:13 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

static int	check_burnouts(t_sim *sim, size_t current_time)
{
	int	i;

	i = -1;
	while (++i < (int)sim->nb_coders)
	{
		if (has_coder_burned_out(&sim->coders[i]))
			return (i);
	}
	(void)current_time;
	return (-1);
}

void	*burnout_monitor(void *arg)
{
	t_sim	*sim;
	int		burnout_coder;

	sim = (t_sim *)arg;
	while (1)
	{
		pthread_mutex_lock(&sim->sim_lock);
		if (!sim->is_running)
		{
			pthread_mutex_unlock(&sim->sim_lock);
			break ;
		}
		burnout_coder = check_burnouts(sim, get_time());
		pthread_mutex_unlock(&sim->sim_lock);
		if (burnout_coder >= 0)
		{
			end_simulation(sim, burnout_coder, 0);
			break ;
		}
		msleep(1);
	}
	return (NULL);
}

static void	broadcast_dongles(t_sim *sim)
{
	int	i;

	i = -1;
	while (++i < (int)sim->nb_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].dongle_lock);
		pthread_cond_broadcast(&sim->dongles[i].dongle_cond);
		pthread_mutex_unlock(&sim->dongles[i].dongle_lock);
	}
}

void	end_simulation(t_sim *sim, int coder_id, int has_printed)
{
	size_t	current_time;

	pthread_mutex_lock(&sim->sim_lock);
	sim->is_running = 0;
	pthread_mutex_unlock(&sim->sim_lock);
	current_time = get_time();
	pthread_mutex_lock(&sim->print_lock);
	if (!has_printed)
		printf("\e[0;31m%zu %d has burned out\e[0;37m\n",
			current_time - sim->start_time, sim->coders[coder_id].id + 1);
	pthread_mutex_unlock(&sim->print_lock);
	broadcast_dongles(sim);
}

int	has_coder_burned_out(t_coder *coder)
{
	pthread_mutex_lock(&coder->coder_lock);
	if (!coder->is_waiting
		&& get_time() - coder->last_compile_start >= coder->sim->burnout_time)
	{
		pthread_mutex_unlock(&coder->coder_lock);
		return (1);
	}
	pthread_mutex_unlock(&coder->coder_lock);
	return (0);
}
