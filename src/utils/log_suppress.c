/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_suppress.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 15:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/22 15:28:49 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Returns a pointer to a hidden suppress flag.
 */
static int	*get_suppress_ptr(void)
{
	static int	g_suppress;

	return (&g_suppress);
}

/*
 * Suppress all future log output (used when burnout is detected
 * to prevent coder messages from appearing after the burnout line).
 */
void	suppress_logs(void)
{
	*get_suppress_ptr() = 1;
}

/*
 * Returns 1 if logging is suppressed, 0 otherwise.
 */
int	is_logging_suppressed(void)
{
	return (*get_suppress_ptr());
}
