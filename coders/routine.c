/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 00:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/05/27 15:15:43 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	int		i;

	coder = (t_coder *)arg;
	i = 0;
	// Dummy loop mimicking the simulation steps purely to test threads creation
	while (i < 3)
	{
		pthread_mutex_lock(&coder->sim->print_lock);
		printf("%zu %d is compiling (dummy test)\n", get_time() - coder->sim->start_time, coder->id + 1);
		pthread_mutex_unlock(&coder->sim->print_lock);
		
		msleep(coder->sim->compile_time);
		
		pthread_mutex_lock(&coder->sim->print_lock);
		printf("%zu %d is debugging (dummy test)\n", get_time() - coder->sim->start_time, coder->id + 1);
		pthread_mutex_unlock(&coder->sim->print_lock);
		
		msleep(coder->sim->debug_time);
		
		pthread_mutex_lock(&coder->sim->print_lock);
		printf("%zu %d is refactoring (dummy test)\n", get_time() - coder->sim->start_time, coder->id + 1);
		pthread_mutex_unlock(&coder->sim->print_lock);
		
		msleep(coder->sim->refactor_time);
		
		i++;
	}
	return (NULL);
}

int	start_simulation(t_sim *sim)
{
	unsigned int	i;

	sim->start_time = get_time();
	sim->is_running = 1;
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!sim->coders)
		return (1);
	
	i = 0;
	// Initialize & Create threads
	while (i < sim->nb_coders)
	{
		sim->coders[i].id = i;
		sim->coders[i].times_compiled = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].sim = sim;
		pthread_mutex_init(&sim->coders[i].coder_lock, NULL);
		
		pthread_create(&sim->coders[i].thread, NULL, coder_routine, &sim->coders[i]);
		i++;
	}
	
	i = 0;
	// Join threads
	while (i < sim->nb_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	return (0);
}
