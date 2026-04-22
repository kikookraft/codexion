/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_adds.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:00:38 by tobesson          #+#    #+#             */
/*   Updated: 2026/04/22 16:44:12 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc.h"

int	heap_init(t_req_heap *heap, size_t capacity)
{
	if (!heap || capacity == 0)
		return (1);
	heap->arr = malloc(sizeof(t_request) * capacity);
	if (!heap->arr)
		return (1);
	heap->size = 0;
	heap->capacity = capacity;
	return (0);
}

int	heap_push(t_req_heap *heap, t_request req, t_sched_mode mode)
{
	if (!heap || !heap->arr || heap->size >= heap->capacity)
		return (1);
	heap->arr[heap->size] = req;
	heap_sift_up(heap, heap->size, mode);
	heap->size++;
	return (0);
}
