/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 00:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/04 14:14:20 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

static int	take_dongle_finish(t_coder *coder, t_dongle *dongle, int ok)
{
	remove_coder(dongle, coder);
	if (!ok)
	{
		pthread_cond_broadcast(&dongle->dongle_cond);
		pthread_mutex_unlock(&dongle->dongle_lock);
		return (0);
	}
	dongle->is_used = 1;
	dongle->last_user = coder->id;
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%-5zu %-5d has taken a dongle\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	pthread_mutex_unlock(&dongle->dongle_lock);
	return (1);
}

static int	should_wait(t_coder *c, t_dongle *d)
{
	if (d->is_used || d->waiting[0] != c)
		return (1);
	if (d->last_user == c->id && d->queue_size == 2)
	{
		d->waiting[0] = d->waiting[1];
		d->waiting[1] = c;
		d->last_user = -1;
		return (1);
	}
	return (0);
}

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	int	running;

	pthread_mutex_lock(&dongle->dongle_lock);
	enqueue_coder(dongle, coder);
	pthread_mutex_lock(&coder->coder_lock);
	coder->is_waiting = 1;
	pthread_mutex_unlock(&coder->coder_lock);
	while (coder->sim->is_running)
	{
		if (should_wait(coder, dongle))
			pthread_cond_wait(&dongle->dongle_cond, &dongle->dongle_lock);
		else if (dongle->last_used + coder->sim->dongle_cooldown > get_time())
			dongle_take_wait(dongle, coder);
		else
			break ;
	}
	running = coder->sim->is_running;
	pthread_mutex_lock(&coder->coder_lock);
	coder->is_waiting = 0;
	pthread_mutex_unlock(&coder->coder_lock);
	return (take_dongle_finish(coder, dongle, running));
}

void	dongle_take_wait(t_dongle *dongle, t_coder *coder)
{
	size_t			now;
	size_t			cooldown_end;
	size_t			remaining;
	struct timespec	ts;

	now = get_time();
	cooldown_end = dongle->last_used + coder->sim->dongle_cooldown;
	if (cooldown_end <= now)
		return ;
	remaining = cooldown_end - now;
	ts.tv_sec = (now / 1000) + (remaining / 1000);
	ts.tv_nsec = ((now % 1000) * 1000000) + ((remaining % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&dongle->dongle_cond, &dongle->dongle_lock, &ts);
}

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
	pthread_mutex_lock(&sim->sim_lock);
	if (sim->is_running)
	{
		sim->is_running = 0;
		pthread_mutex_unlock(&sim->sim_lock);
		pthread_mutex_lock(&sim->print_lock);
		printf("%sSimulation complete: all coders compiled %d times.%s\n",
			"\e[0;32m", sim->target_compiles, "\e[0;37m");
		pthread_mutex_unlock(&sim->print_lock);
	}
	pthread_mutex_unlock(&sim->sim_lock);
	pthread_join(sim->burnout_thread, NULL);
	return (0);
}
