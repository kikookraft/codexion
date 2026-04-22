/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_operation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 14:07:42 by tobesson          #+#    #+#             */
/*   Updated: 2026/04/22 16:44:23 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc.h"

int	heap_pop(t_req_heap *heap, t_request *out, t_sched_mode mode)
{
	if (!heap || !heap->arr || heap->size == 0)
		return (1);
	if (out)
		*out = heap->arr[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->arr[0] = heap->arr[heap->size];
		heap_sift_down(heap, 0, mode);
	}
	return (0);
}
