/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 00:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/02 14:47:21 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_lock);
	enqueue_coder(dongle, coder);
	pthread_mutex_lock(&coder->coder_lock);
	coder->is_waiting = 1;
	pthread_mutex_unlock(&coder->coder_lock);
	while (1)
	{
		if (dongle->is_used || dongle->waiting[0] != coder)
			pthread_cond_wait(&dongle->dongle_cond, &dongle->dongle_lock);
		else
			if (dongle->last_used + coder->sim->dongle_cooldown > get_time())
				dongle_take_wait(dongle, coder);
			else
				break ;
	}
	pthread_mutex_lock(&coder->coder_lock);
	coder->is_waiting = 0;
	pthread_mutex_unlock(&coder->coder_lock);
	remove_coder(dongle, coder);
	dongle->is_used = 1;
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%zu %d has taken a dongle\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	pthread_mutex_unlock(&dongle->dongle_lock);
}

void	dongle_take_wait(t_dongle *dongle, t_coder *coder)
{
	size_t			cooldown_end;
	struct timeval	tv;
	struct timespec	ts;

	cooldown_end = dongle->last_used + coder->sim->dongle_cooldown;
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + ((cooldown_end - get_time()) / 1000);
	ts.tv_nsec = (tv.tv_usec * 1000)
		+ (((cooldown_end - get_time()) % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&dongle->dongle_cond, &dongle->dongle_lock, &ts);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_lock);
	dongle->is_used = 0;
	dongle->last_used = get_time();
	pthread_cond_broadcast(&dongle->dongle_cond);
	pthread_mutex_unlock(&dongle->dongle_lock);
}

int	start_simulation(t_sim *sim)
{
	int	i;

	sim->start_time = get_time();
	sim->is_running = 1;
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!sim->coders)
		return (1);
	pthread_create(&sim->burnout_thread, NULL, burnout_monitor, sim);
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
	i = -1;
	while ((unsigned int)++i < sim->nb_coders)
		pthread_join(sim->coders[i].thread, NULL);
	pthread_join(sim->burnout_thread, NULL);
	return (0);
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
