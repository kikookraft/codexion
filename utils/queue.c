/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 12:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/01 15:06:45 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/* Swap two coders in the queue */
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
		// FIFO: arrival order. But if they arrive at the exact same moment, tie break by id.
		// Since enqueue happens sequentially, c1 was here first. 
		// We only swap if we REALLY enforce tie breaking on same arrival time, but realistically 
		// the one already in the queue `c1` was requested earlier. 
		// But let's follow the strict "tie break by ID" rule even for FIFO if somehow simultaneous?
		// Usually FIFO is just strictly arrival order. Let's keep it strictly arrival order, unless their arrival is identical?
		// Arrival order is guaranteed by queue insertion order. So no swap under pure FIFO.
		return (0); 
	}
	else // EDF
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

/* Enqueue a coder based on arbitration rules */
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
		
		// Priority sort: index 0 should be the one who gets the dongle next
		if (should_swap(dongle->waiting[0], dongle->waiting[1], coder->sim->scheduler))
		{
			swap_coders(dongle);
		}
	}
}

/* Remove a specific coder from the queue */
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
