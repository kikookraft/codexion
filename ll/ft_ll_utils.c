/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 14:20:41 by tobesson          #+#    #+#             */
/*   Updated: 2026/01/20 17:03:37 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// exchange two element of ll1 & ll2
void	ll_exchange(t_ll **ll1, t_ll **ll2)
{
	int	temp;

	if (!*ll1 || !*ll2)
		return ;
	temp = (*ll1)->value;
	(*ll1)->value = (*ll2)->value;
	(*ll2)->value = temp;
}

// get the node at given index
t_ll	*get_index(int index, t_ll **ll)
{
	int		i;
	t_ll	*temp;

	temp = *ll;
	i = -1;
	while (++i < index && temp)
		temp = temp->next;
	if (!temp)
		return (NULL);
	return (temp);
}
