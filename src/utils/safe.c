/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 14:50:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/22 15:28:49 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Safe wrapper for pthread_mutex_lock.
 * Returns 0 on success, 1 on error.
 */
int	safe_mutex_lock(pthread_mutex_t *m)
{
	if (pthread_mutex_lock(m) != 0)
		return (1);
	return (0);
}

/*
 * Safe wrapper for pthread_mutex_unlock.
 * Returns 0 on success, 1 on error.
 */
int	safe_mutex_unlock(pthread_mutex_t *m)
{
	if (pthread_mutex_unlock(m) != 0)
		return (1);
	return (0);
}

/*
 * Safe wrapper for pthread_mutex_init.
 * Returns 0 on success, 1 on error.
 */
int	safe_mutex_init(pthread_mutex_t *m)
{
	if (pthread_mutex_init(m, NULL) != 0)
		return (1);
	return (0);
}

/*
 * Safe wrapper for pthread_cond_init.
 * Returns 0 on success, 1 on error.
 */
int	safe_cond_init(pthread_cond_t *c)
{
	if (pthread_cond_init(c, NULL) != 0)
		return (1);
	return (0);
}

/*
 * Safe wrapper for pthread_create.
 * Returns 0 on success, 1 on error.
 */
int	safe_thread_create(pthread_t *t, void *(*f)(void *), void *arg)
{
	if (pthread_create(t, NULL, f, arg) != 0)
		return (1);
	return (0);
}
