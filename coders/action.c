/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:32:49 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/02 16:59:42 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_dongle	*donglel;
	t_dongle	*dongler;
	int			i;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
	{
		donglel = &coder->sim->dongles[coder->id];
		dongler = &coder->sim->dongles[(coder->id + 1) % coder->sim->nb_coders];
	}
	else
	{
		donglel = &coder->sim->dongles[(coder->id + 1) % coder->sim->nb_coders];
		dongler = &coder->sim->dongles[coder->id];
	}
	i = -1;
	while ((unsigned int)++i < coder->sim->target_compiles
		&& coder->sim->is_running)
	{
		compile(coder, donglel, dongler);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}

void	compile(t_coder *coder, t_dongle *l, t_dongle *r)
{
	if (!take_dongle(coder, l))
		return ;
	if (!take_dongle(coder, r))
	{
		release_dongle(l);
		return ;
	}
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%zu %d is compiling\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_compile_start = get_time();
	pthread_mutex_unlock(&coder->coder_lock);
	msleep(coder->sim->compile_time);
	release_dongle(r);
	release_dongle(l);
	pthread_mutex_lock(&coder->coder_lock);
	coder->times_compiled++;
	pthread_mutex_unlock(&coder->coder_lock);
}

void	debug(t_coder *coder)
{
	if (!coder->sim->is_running)
		return ;
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%zu %d is debugging\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	if (!coder->sim->is_running)
		return ;
	msleep(coder->sim->debug_time);
}

void	refactor(t_coder *coder)
{
	if (!coder->sim->is_running)
		return ;
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%zu %d is refactoring\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	if (!coder->sim->is_running)
		return ;
	msleep(coder->sim->refactor_time);
}
