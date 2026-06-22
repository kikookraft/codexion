/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:35:48 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/22 15:28:49 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Allocates the simulation structure, parses all numeric arguments,
 * initializes dongles, mutexes, and the scheduler.
 * On any failure, cleans up and returns NULL.
 */
t_sim	*init_sim(char *argv[])
{
	t_sim	*sim;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	memset(sim, 0, sizeof(t_sim));
	sim->burned_out_coder = -1;
	parse_sim_args(sim, argv);
	if (safe_mutex_init(&sim->print_lock)
		|| safe_mutex_init(&sim->sim_lock))
	{
		free(sim);
		return (NULL);
	}
	sim->dongles = init_dongle(sim->nb_coders);
	sim->scheduler = init_scheduler(argv);
	if (!sim->dongles)
	{
		pthread_mutex_destroy(&sim->print_lock);
		pthread_mutex_destroy(&sim->sim_lock);
		free(sim);
		return (NULL);
	}
	return (sim);
}

/*
 * Initializes miscellaneous simulation parameters.
 */
void	init_misc(t_sim *sim)
{
	set_simulation_start_time(get_time());
	print_set_mutex(&sim->print_lock);
}

/*
 * Parses the scheduler argument: returns FIFO if "fifo", else EDF.
 */
t_scheduler	init_scheduler(char *argv[])
{
	t_scheduler	scheduler;

	if (strcmp(argv[8], "fifo") == 0)
		scheduler = FIFO;
	else
		scheduler = EDF;
	return (scheduler);
}
