/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_size.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlaurens <rlaurens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:29:56 by tobesson          #+#    #+#             */
/*   Updated: 2026/01/22 13:57:41 by rlaurens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// return the size of the linked list
int	ll_size(t_ll **ll)
{
	t_ll	*temp;
	int		size;

	temp = *ll;
	size = 0;
	while (temp)
	{
		size++;
		temp = temp->next;
	}
	return (size);
}

// return the last node of the linked list
t_ll	**get_last_node(t_ll **ll)
{
	t_ll	**temp;

	if (!*ll)
		return (NULL);
	temp = ll;
	while ((*temp)->next)
		temp = &(*temp)->next;
	return (temp);
}
