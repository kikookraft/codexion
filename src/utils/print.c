/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 12:05:39 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/17 12:28:13 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Prints a message under print_lock for serialized output.
 */
void	log_term(char *message, t_sim *sim)
{
	pthread_mutex_lock(&sim->print_lock);
	printf("%s\n", message);
	pthread_mutex_unlock(&sim->print_lock);
}

/*
 * Called after all coder threads finish. If no burnout occurred,
 * prints the success message and marks the simulation as ended.
 */
void	sim_ended(t_sim *sim)
{
	pthread_mutex_lock(&sim->sim_lock);
	if (sim->is_running)
	{
		sim->is_running = 0;
		pthread_mutex_unlock(&sim->sim_lock);
		printf("%sSimulation complete: all coders compiled %d times.%s\n",
			"\e[0;32m", sim->target_compiles, "\e[0;37m");
	}
	else
		pthread_mutex_unlock(&sim->sim_lock);
}
