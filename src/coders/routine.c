/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 00:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 18:43:16 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Thread entry point for each coder. Applies adaptive startup stagger
 * (even-first for even coder counts, odd-first for odd counts),
 * resolves the coder's left/right dongle pair using alternating ordering
 * to prevent deadlock, then enters the compile-debug-refactor loop.
 */
void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_dongle	*donglel;
	t_dongle	*dongler;

	coder = (t_coder *)arg;
	if (coder->sim->nb_coders % 2 == 0 && coder->id % 2 != 0)
		usleep(500);
	else if (coder->sim->nb_coders % 2 != 0 && coder->id % 2 == 0)
		usleep(500);
	if (coder->id % 2 == 0)
	{
		donglel = &coder->sim->dongles[coder->id];
		dongler = &coder->sim->dongles[(coder->id + 1)
			% coder->sim->nb_coders];
	}
	else
	{
		donglel = &coder->sim->dongles[(coder->id + 1)
			% coder->sim->nb_coders];
		dongler = &coder->sim->dongles[coder->id];
	}
	routine_loop(coder, donglel, dongler);
	return (NULL);
}

/*
 * Returns 1 if the coder should wait for this dongle:
 * - dongle is in use (is_used)
 * - coder is not first in priority queue (waiting[0])
 * - fairness bump: coder just used the dongle and another coder is waiting
 */
int	should_wait(t_coder *c, t_dongle *d)
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


/*
 * Helper: calls pthread_cond_timedwait with an absolute deadline.
 * Returns 1 if the wait timed out (ETIMEDOUT), 0 if signaled.
 */
int	timedwait_or_timeout(t_dongle *dongle, size_t deadline)
{
	struct timespec	ts;
	int				ret;

	ts.tv_sec = deadline / 1000;
	ts.tv_nsec = (deadline % 1000) * 1000000;
	ret = pthread_cond_timedwait(&dongle->dongle_cond,
			&dongle->dongle_lock, &ts);
	return (ret == ETIMEDOUT);
}

