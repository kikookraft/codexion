/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:35:48 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/02 11:57:41 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

t_dongle	*init_dongle(int nb_coders)
{
	t_dongle	*dongle;
	int			i;

	dongle = malloc(sizeof(t_dongle) * nb_coders);
	if (!dongle)
		return (NULL);
	i = -1;
	while (++i < nb_coders)
	{
		dongle[i].dongle_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
		dongle[i].dongle_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
		dongle[i].id = i;
		dongle[i].is_used = 0;
		dongle[i].last_used = 0;
		dongle[i].queue_size = 0;
		dongle[i].waiting[0] = NULL;
		dongle[i].waiting[1] = NULL;
	}
	return (dongle);
}

t_scheduler	init_scheduler(char *argv[])
{
	t_scheduler	scheduler;

	if (strcmp(argv[8], "fifo") == 0)
		scheduler = FIFO;
	else
		scheduler = EDF;
	return (scheduler);
}

t_sim	*init_sim(char *argv[])
{
	t_sim	*sim;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	sim->nb_coders = atoi(argv[1]);
	sim->burnout_time = atoi(argv[2]);
	sim->compile_time = atoi(argv[3]);
	sim->debug_time = atoi(argv[4]);
	sim->refactor_time = atoi(argv[5]);
	sim->target_compiles = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	sim->is_running = 0;
	sim->start_time = 0;
	sim->dongles = init_dongle(sim->nb_coders);
	sim->print_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	sim->sim_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	sim->scheduler = init_scheduler(argv);
	if (!sim->dongles)
	{
		free(sim);
		return (NULL);
	}
	return (sim);
}
