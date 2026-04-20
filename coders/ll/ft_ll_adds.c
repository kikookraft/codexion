/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_adds.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlaurens <rlaurens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:00:38 by tobesson          #+#    #+#             */
/*   Updated: 2026/01/22 13:57:32 by rlaurens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// create a new empty linked list
t_ll	**ll_new_empty(void)
{
	t_ll	**new;

	new = malloc(sizeof(t_ll *));
	if (!new)
		return (NULL);
	*new = NULL;
	return (new);
}

// put value at the bottom of the stack (at the end of the linked list)
int	ll_add_back(t_ll **ll, int value)
{
	t_ll	*new_node;
	t_ll	**last;

	new_node = malloc(sizeof(t_ll));
	if (!new_node)
		return (0);
	new_node->value = value;
	new_node->next = NULL;
	new_node->prev = NULL;
	if (!*ll)
	{
		*ll = new_node;
		return (1);
	}
	last = get_last_node(ll);
	(*last)->next = new_node;
	new_node->prev = *last;
	return (1);
}
