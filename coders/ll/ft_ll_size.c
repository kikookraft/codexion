/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_size.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:29:56 by tobesson          #+#    #+#             */
/*   Updated: 2026/04/22 16:44:24 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc.h"

size_t	heap_size(t_req_heap *heap)
{
	if (!heap)
		return (0);
	return (heap->size);
}

int	heap_is_empty(t_req_heap *heap)
{
	if (!heap)
		return (1);
	return (heap->size == 0);
}
