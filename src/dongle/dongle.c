/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 17:02:59 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/23 18:32:09 by tobesson         ###   ########.fr       */
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

/*
 * Blocking dongle acquisition. Enqueues the coder in the priority
 * queue, then waits until the dongle is free, cooldown has expired,
 * and the coder is first in queue. Used for left dongle (always)
 * and right dongle when coder count is even.
 */
int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	int	running;

	safe_mutex_lock(&dongle->dongle_lock);
	enqueue_coder(dongle, coder);
	safe_mutex_lock(&coder->coder_lock);
	coder->is_waiting = 1;
	safe_mutex_unlock(&coder->coder_lock);
	while (is_simulation_running(coder->sim))
	{
		if (should_wait(coder, dongle))
			pthread_cond_wait(&dongle->dongle_cond,
				&dongle->dongle_lock);
		else if (dongle->last_used
			+ coder->sim->dongle_cooldown > get_time())
			dongle_take_wait(dongle, coder);
		else
			break ;
	}
	running = is_simulation_running(coder->sim);
	safe_mutex_lock(&coder->coder_lock);
	coder->is_waiting = 0;
	safe_mutex_unlock(&coder->coder_lock);
	return (take_dongle_finish(coder, dongle, running));
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

	safe_mutex_lock(&dongle->dongle_lock);
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

/*
 * Finalizes dongle acquisition: removes the coder from the queue,
 * marks the dongle as used, prints "has taken a dongle" under
 * print_lock. Returns 0 if simulation stopped mid-acquisition.
 */
int	take_dongle_finish(t_coder *coder, t_dongle *dongle, int ok)
{
	remove_coder(dongle, coder);
	if (!ok)
	{
		pthread_cond_broadcast(&dongle->dongle_cond);
		safe_mutex_unlock(&dongle->dongle_lock);
		return (0);
	}
	dongle->is_used = 1;
	dongle->last_user = coder->id;
	log_action("has taken a dongle", coder->id);
	safe_mutex_unlock(&dongle->dongle_lock);
	return (1);
}

/*
 * Releases a dongle: marks it unused, records the release timestamp
 * for cooldown tracking, broadcasts the condition variable inside
 * the mutex lock (helgrind-clean), then unlocks.
 */
void	release_dongle(t_dongle *dongle)
{
	safe_mutex_lock(&dongle->dongle_lock);
	dongle->is_used = 0;
	dongle->last_used = get_time();
	pthread_cond_broadcast(&dongle->dongle_cond);
	safe_mutex_unlock(&dongle->dongle_lock);
}
