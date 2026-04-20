/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ll_operation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 14:07:42 by tobesson          #+#    #+#             */
/*   Updated: 2026/01/16 14:32:45 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

// Perform the operation 'op' on stack_a and stack_b, and record it in ops
int	operate(t_ll **stack_a, t_ll **stack_b, t_ll **ops, int op)
{
	if (op == OP_SA)
		swap(stack_a);
	if (op == OP_SB)
		swap(stack_b);
	if (op == OP_SS)
		both(&swap, stack_a, stack_b);
	if (op == OP_PA)
		push(stack_b, stack_a);
	if (op == OP_PB)
		push(stack_a, stack_b);
	if (op == OP_RA)
		rotate(stack_a);
	if (op == OP_RB)
		rotate(stack_b);
	if (op == OP_RR)
		both(&rotate, stack_a, stack_b);
	if (op == OP_RRA)
		rotate_reverse(stack_a);
	if (op == OP_RRB)
		rotate_reverse(stack_b);
	if (op == OP_RRR)
		both(&rotate_reverse, stack_a, stack_b);
	if (!ll_add_back(ops, op))
		return (0);
	return (1);
}
