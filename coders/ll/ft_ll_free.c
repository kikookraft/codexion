/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:13:40 by tobesson          #+#    #+#             */
/*   Updated: 2026/04/22 16:44:20 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc.h"

void	heap_destroy(t_req_heap *heap)
{
	if (!heap)
		return ;
	free(heap->arr);
	heap->arr = NULL;
	heap->size = 0;
	heap->capacity = 0;
}
