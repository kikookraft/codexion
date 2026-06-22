/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 15:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/22 15:28:49 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Cleans up dongle mutexes and conditions on init failure.
 */
static void	cleanup_dongles(t_dongle *dongle, int count)
{
	int	i;

	i = -1;
	while (++i < count)
	{
		pthread_mutex_destroy(&dongle[i].dongle_lock);
		pthread_cond_destroy(&dongle[i].dongle_cond);
	}
	free(dongle);
}

/*
 * Allocates and initializes an array of nb_coders dongles.
 * Each dongle gets its own mutex and condition variable.
 * On failure, cleans up previously initialized items and returns NULL.
 */
t_dongle	*init_dongle(int nb_coders)
{
	t_dongle	*dongle;
	int			i;

	dongle = malloc(sizeof(t_dongle) * nb_coders);
	if (!dongle)
		return (NULL);
	i = -1;
	while (++i < nb_coders)
	{
		dongle[i].id = i;
		dongle[i].is_used = 0;
		dongle[i].last_user = -1;
		dongle[i].last_used = 0;
		dongle[i].queue_size = 0;
		dongle[i].waiting[0] = NULL;
		dongle[i].waiting[1] = NULL;
		if (safe_mutex_init(&dongle[i].dongle_lock)
			|| safe_cond_init(&dongle[i].dongle_cond))
		{
			cleanup_dongles(dongle, i);
			return (NULL);
		}
	}
	return (dongle);
}
