/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   burnout.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 12:32:36 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/02 14:53:26 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

void	*burnout_monitor(void *arg)
{
	t_sim	*sim;
	size_t	current_time;
	int		i;

	sim = (t_sim *)arg;
	while (1)
	{
		pthread_mutex_lock(&sim->sim_lock);
		if (!sim->is_running)
		{
			pthread_mutex_unlock(&sim->sim_lock);
			break ;
		}
		current_time = get_time();
		i = -1;
		while (++i < (int)sim->nb_coders)
		{
			if (has_coder_burned_out(&sim->coders[i]))
			{
				end_simulation(sim, i, 0);
				break ;
			}
		}
		pthread_mutex_unlock(&sim->sim_lock);
		msleep(1);
	}
	return (NULL);
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
		printf("%zu %d has burned out\n",
			current_time - sim->start_time, sim->coders[coder_id].id + 1);
	pthread_mutex_unlock(&sim->print_lock);
	return ;
}
