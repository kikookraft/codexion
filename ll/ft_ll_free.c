/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlaurens <rlaurens@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 12:13:40 by tobesson          #+#    #+#             */
/*   Updated: 2026/01/22 14:11:01 by rlaurens         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// free all nodes in the linked list and
void	ll_free(t_ll **ll)
{
	t_ll	*temp;
	t_ll	*next;

	if (!ll)
		return ;
	temp = *ll;
	while (temp)
	{
		next = temp->next;
		free(temp);
		temp = next;
	}
	free(ll);
}
