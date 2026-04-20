/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_move.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:33:09 by tobesson          #+#    #+#             */
/*   Updated: 2026/01/16 14:33:09 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// swap the first two elements of the linked list ll
void	swap(t_ll **ll)
{
	if (!*ll || !(*ll)->next)
		return ;
	ll_exchange(ll, &(*ll)->next);
}

// move the first element of ll_src to the top of ll_dst
void	push(t_ll **ll_src, t_ll **ll_dst)
{
	t_ll	*temp;

	if (!*ll_src)
		return ;
	temp = *ll_src;
	*ll_src = (*ll_src)->next;
	temp->next = *ll_dst;
	if (*ll_dst)
		(*ll_dst)->prev = temp;
	*ll_dst = temp;
}

// rotate the linked list ll upwards (first element becomes last)
void	rotate(t_ll **ll)
{
	t_ll	*first;
	t_ll	*last;

	if (!*ll || !(*ll)->next)
		return ;
	first = *ll;
	last = *get_last_node(ll);
	*ll = (*ll)->next;
	first->next = NULL;
	last->next = first;
	first->prev = last;
	(*ll)->prev = NULL;
}

// rotate the linked list ll downwards (last element becomes first)
void	rotate_reverse(t_ll **ll)
{
	t_ll	*temp;
	t_ll	*last;

	if (!ll || !(*ll)->next)
		return ;
	last = *get_last_node(ll);
	temp = last;
	temp->next = *ll;
	(*ll)->prev = temp;
	(*ll) = temp;
	temp->prev->next = NULL;
	(*ll)->prev = NULL;
}

// execute afunction f on both linked lists ll1 and ll2
void	both(void (*f)(t_ll **), t_ll **ll1, t_ll **ll2)
{
	f(ll1);
	f(ll2);
}
