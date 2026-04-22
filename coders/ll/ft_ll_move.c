/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_move.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:33:09 by tobesson          #+#    #+#             */
/*   Updated: 2026/04/22 16:44:23 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc.h"

void	heap_swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heap_sift_up(t_req_heap *heap, size_t idx, t_sched_mode mode)
{
	size_t	parent;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (request_cmp(&heap->arr[idx], &heap->arr[parent], mode) >= 0)
			break ;
		heap_swap(&heap->arr[idx], &heap->arr[parent]);
		idx = parent;
	}
}

void	heap_sift_down(t_req_heap *heap, size_t idx, t_sched_mode mode)
{
	size_t	left;
	size_t	right;
	size_t	smallest;

	while (1)
	{
		left = (idx * 2) + 1;
		right = (idx * 2) + 2;
		smallest = idx;
		if (left < heap->size
			&& request_cmp(&heap->arr[left], &heap->arr[smallest], mode) < 0)
			smallest = left;
		if (right < heap->size
			&& request_cmp(&heap->arr[right], &heap->arr[smallest], mode) < 0)
			smallest = right;
		if (smallest == idx)
			break ;
		heap_swap(&heap->arr[idx], &heap->arr[smallest]);
		idx = smallest;
	}
}
