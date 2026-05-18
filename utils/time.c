/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 16:11:50 by tobesson          #+#    #+#             */
/*   Updated: 2026/05/18 16:26:08 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

size_t	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000 + tv.tv_usec);
}

/* Busy wait for a specified time in microseconds */
void	umimir(size_t time)
{
	size_t	start;

	start = get_time();
	while (get_time() - start < time)
	{
	}
}

/* Busy wait for a specified time in milliseconds */
void	mimir(size_t time)
{
	umimir(time * 1000);
}
