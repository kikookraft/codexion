/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:57:17 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/16 15:42:58 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

size_t	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((size_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

void	msleep(size_t time)
{
	size_t	start;

	if (time == 0)
		return ;
	start = get_time();
	if (time > 2)
		usleep((time - 2) * 1000);
	while ((get_time() - start) < time)
		usleep(200);
}

int	is_simulation_running(t_sim *sim)
{
	int	running;

	pthread_mutex_lock(&sim->sim_lock);
	running = sim->is_running;
	pthread_mutex_unlock(&sim->sim_lock);
	return (running);
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
	ts.tv_nsec = ((now % 1000) * 1000000)
		+ ((remaining % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&dongle->dongle_cond,
		&dongle->dongle_lock, &ts);
}
