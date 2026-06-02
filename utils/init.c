/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:35:48 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/02 17:07:14 by tobesson         ###   ########.fr       */
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
		pthread_cond_init(&dongle[i].dongle_cond, NULL);
		pthread_mutex_init(&dongle[i].dongle_lock, NULL);
		dongle[i].id = i;
		dongle[i].is_used = 0;
		dongle[i].last_user = -1;
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
	pthread_mutex_init(&sim->print_lock, NULL);
	pthread_mutex_init(&sim->sim_lock, NULL);
	sim->scheduler = init_scheduler(argv);
	sim->burnout_thread = (pthread_t)0;
	if (!sim->dongles)
	{
		free(sim);
		return (NULL);
	}
	return (sim);
}

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

void	init_coders(t_sim *sim)
{
	int	i;

	i = -1;
	while ((unsigned int)++i < sim->nb_coders)
	{
		sim->coders[i].id = (unsigned int)i;
		sim->coders[i].times_compiled = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].sim = sim;
		sim->coders[i].is_waiting = 0;
		pthread_mutex_init(&sim->coders[i].coder_lock, NULL);
		pthread_create(&sim->coders[i].thread,
			NULL, coder_routine, &sim->coders[i]);
	}
}
