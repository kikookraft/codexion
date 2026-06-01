/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:32:49 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/01 17:16:41 by tobesson         ###   ########.fr       */
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
	i = -1;
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
	while ((unsigned int)++i < coder->sim->target_compiles)
	{
		compile(coder, donglel, dongler);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}

void	compile(t_coder *coder, t_dongle *left_dongle, t_dongle *right_dongle)
{
	take_dongle(coder, left_dongle);
	take_dongle(coder, right_dongle);
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%zu %d is compiling\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_compile_start = get_time();
	pthread_mutex_unlock(&coder->coder_lock);
	if (!coder->sim->is_running)
		return ;
	msleep(coder->sim->compile_time);
	if (!coder->sim->is_running)
		return ;
	release_dongle(right_dongle);
	release_dongle(left_dongle);
	pthread_mutex_lock(&coder->coder_lock);
	coder->times_compiled++;
	pthread_mutex_unlock(&coder->coder_lock);
}

void	debug(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%zu %d is debugging\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	if (!coder->sim->is_running)
		return ;
	msleep(coder->sim->debug_time);
	if (!coder->sim->is_running)
		return ;
}

void	refactor(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%zu %d is refactoring\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	if (!coder->sim->is_running)
		return ;
	msleep(coder->sim->refactor_time);
	if (!coder->sim->is_running)
		return ;
}
