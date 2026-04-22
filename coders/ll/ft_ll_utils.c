/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 14:20:41 by tobesson          #+#    #+#             */
/*   Updated: 2026/04/22 16:44:25 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc.h"

int	heap_peek(t_req_heap *heap, t_request *out)
{
	if (!heap || !out || heap->size == 0)
		return (1);
	*out = heap->arr[0];
	return (0);
}

int	request_cmp(const t_request *a, const t_request *b, t_sched_mode mode)
{
	if (mode == COD_SCHED_EDF)
	{
		if (a->deadline_ms < b->deadline_ms)
			return (-1);
		if (a->deadline_ms > b->deadline_ms)
			return (1);
	}
	if (a->seq < b->seq)
		return (-1);
	if (a->seq > b->seq)
		return (1);
	return (0);
}
