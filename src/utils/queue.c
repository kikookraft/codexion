/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 12:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 18:33:40 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Swaps waiting[0] and waiting[1] after EDF reordering.
 */
static void	swap_coders(t_dongle *dongle)
{
	t_coder	*tmp;

	tmp = dongle->waiting[0];
	dongle->waiting[0] = dongle->waiting[1];
	dongle->waiting[1] = tmp;
}

/* 
 * Tie breaker: Coder ID (lower value first)
 * Comparison logic:
 * Returns 1 if c2 should be BEFORE c1 (meaning we need to swap them)
 */
static int	should_swap(t_coder *c1, t_coder *c2, t_scheduler scheduler)
{
	size_t	deadline1;
	size_t	deadline2;

	if (scheduler == FIFO)
	{
		return (0);
	}
	else
	{
		deadline1 = c1->last_compile_start + c1->sim->burnout_time;
		deadline2 = c2->last_compile_start + c2->sim->burnout_time;
		if (deadline2 < deadline1)
			return (1);
		else if (deadline2 == deadline1 && c2->id < c1->id)
			return (1);
		return (0);
	}
}

/*
 * Adds a coder to the dongle's 2-slot waitlist. If a second coder
 * arrives, calls should_swap to reorder by scheduler policy
 * (FIFO preserves order; EDF sorts by earliest deadline).
 */
void	enqueue_coder(t_dongle *dongle, t_coder *coder)
{
	if (dongle->queue_size == 0)
	{
		dongle->waiting[0] = coder;
		dongle->queue_size = 1;
	}
	else if (dongle->queue_size == 1)
	{
		dongle->waiting[1] = coder;
		dongle->queue_size = 2;
		if (should_swap(dongle->waiting[0], dongle->waiting[1],
				coder->sim->scheduler))
		{
			swap_coders(dongle);
		}
	}
}

/*
 * Removes a specific coder from the waitlist, shifting slot 1
 * to slot 0 if needed.
 */
void	remove_coder(t_dongle *dongle, t_coder *coder)
{
	if (dongle->queue_size == 2)
	{
		if (dongle->waiting[0] == coder)
		{
			dongle->waiting[0] = dongle->waiting[1];
			dongle->waiting[1] = NULL;
			dongle->queue_size = 1;
		}
		else if (dongle->waiting[1] == coder)
		{
			dongle->waiting[1] = NULL;
			dongle->queue_size = 1;
		}
	}
	else if (dongle->queue_size == 1 && dongle->waiting[0] == coder)
	{
		dongle->waiting[0] = NULL;
		dongle->queue_size = 0;
	}
}
