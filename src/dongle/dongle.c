/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 17:02:59 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 17:03:06 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Called when a dongle is free but in cooldown. Computes the
 * absolute cooldown expiry time and calls pthread_cond_timedwait.
 */
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
