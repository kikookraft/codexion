/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 00:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 16:56:09 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Finalizes dongle acquisition: removes the coder from the queue,
 * marks the dongle as used, prints "has taken a dongle" under
 * print_lock. Returns 0 if simulation stopped mid-acquisition.
 */
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
	if (!coder->sim->is_running)
	{
		pthread_cond_broadcast(&dongle->dongle_cond);
		pthread_mutex_unlock(&dongle->dongle_lock);
		return (0);
	}
	log_action("has taken a dongle", coder->id,
		get_time() - coder->sim->start_time);
	pthread_mutex_unlock(&dongle->dongle_lock);
	return (1);
}

/*
 * Returns 1 if the coder should wait for this dongle:
 * - dongle is in use (is_used)
 * - coder is not first in priority queue (waiting[0])
 * - fairness bump: coder just used the dongle and another coder is waiting
 */
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

/*
 * Blocking dongle acquisition. Enqueues the coder in the priority
 * queue, then waits until the dongle is free, cooldown has expired,
 * and the coder is first in queue. Used for left dongle (always)
 * and right dongle when coder count is even.
 */
int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	int	running;

	pthread_mutex_lock(&dongle->dongle_lock);
	enqueue_coder(dongle, coder);
	pthread_mutex_lock(&coder->coder_lock);
	coder->is_waiting = 1;
	pthread_mutex_unlock(&coder->coder_lock);
	while (is_simulation_running(coder->sim))
	{
		if (should_wait(coder, dongle))
			pthread_cond_wait(&dongle->dongle_cond, &dongle->dongle_lock);
		else if (dongle->last_used + coder->sim->dongle_cooldown > get_time())
			dongle_take_wait(dongle, coder);
		else
			break ;
	}
	running = is_simulation_running(coder->sim);
	pthread_mutex_lock(&coder->coder_lock);
	coder->is_waiting = 0;
	pthread_mutex_unlock(&coder->coder_lock);
	return (take_dongle_finish(coder, dongle, running));
}

/*
 * Helper: calls pthread_cond_timedwait with an absolute deadline.
 * Returns 1 if the wait timed out (ETIMEDOUT), 0 if signaled.
 */
static int	timedwait_or_timeout(t_dongle *dongle, size_t deadline)
{
	struct timespec	ts;
	int				ret;

	ts.tv_sec = deadline / 1000;
	ts.tv_nsec = (deadline % 1000) * 1000000;
	ret = pthread_cond_timedwait(&dongle->dongle_cond,
			&dongle->dongle_lock, &ts);
	return (ret == ETIMEDOUT);
}

/*
 * Timeout-based dongle acquisition (used for odd coder counts).
 * Same logic as take_dongle but uses pthread_cond_timedwait with
 * an absolute deadline. Returns 0 on timeout so the caller can
 * release the other dongle and retry, preventing hostage cascades.
 */
int	take_dongle_timeout(t_coder *coder, t_dongle *dongle, size_t timeout_ms)
{
	size_t	deadline;

	pthread_mutex_lock(&dongle->dongle_lock);
	enqueue_coder(dongle, coder);
	deadline = get_time() + timeout_ms;
	while (is_simulation_running(coder->sim))
	{
		if (should_wait(coder, dongle))
		{
			if (timedwait_or_timeout(dongle, deadline))
				return (take_dongle_finish(coder, dongle, 0));
		}
		else if (dongle->last_used
			+ coder->sim->dongle_cooldown > get_time())
		{
			if (dongle->last_used
				+ coder->sim->dongle_cooldown > deadline)
				return (take_dongle_finish(coder, dongle, 0));
			dongle_take_wait(dongle, coder);
		}
		else
			break ;
	}
	return (take_dongle_finish(coder, dongle, 1));
}
