/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:32:49 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/17 12:28:13 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Executes the compile phase once both dongles are held:
 * sets last_compile_start BEFORE printing (avoids race with
 * burnout monitor), prints "is compiling", sleeps compile_time ms,
 * releases both dongles, increments times_compiled.
 */
static void	compile_execute(t_coder *coder, t_dongle *l, t_dongle *r)
{
	if (!is_simulation_running(coder->sim))
		return ;
	pthread_mutex_lock(&coder->sim->print_lock);
	if (!is_simulation_running(coder->sim))
	{
		pthread_mutex_unlock(&coder->sim->print_lock);
		release_dongle(r);
		release_dongle(l);
		return ;
	}
	pthread_mutex_lock(&coder->coder_lock);
	coder->last_compile_start = get_time();
	pthread_mutex_unlock(&coder->coder_lock);
	printf("%-5zu %-5d is compiling\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	msleep(coder->sim->compile_time);
	release_dongle(r);
	release_dongle(l);
	pthread_mutex_lock(&coder->coder_lock);
	coder->times_compiled++;
	pthread_mutex_unlock(&coder->coder_lock);
}

/*
 * Acquires both dongles then delegates to compile_execute.
 * Even coder counts: blocking take_dongle on both (hold-and-wait).
 * Odd coder counts: take_dongle_timeout on right dongle with 5 ms
 * deadline and exponential backoff (1-10 ms) to prevent hostage.
 */
void	compile(t_coder *coder, t_dongle *l, t_dongle *r)
{
	int	backoff;

	if (coder->sim->nb_coders % 2 == 0)
	{
		if (!take_dongle(coder, l))
			return ;
		if (!take_dongle(coder, r) && (release_dongle(l), 1))
			return ;
	}
	else
	{
		backoff = 1000;
		while (is_simulation_running(coder->sim))
		{
			if (!take_dongle(coder, l))
				return ;
			if (take_dongle_timeout(coder, r, 5000))
				break ;
			release_dongle(l);
			usleep(backoff);
			if (backoff < 10000)
				backoff *= 2;
		}
	}
	compile_execute(coder, l, r);
}

/*
 * Debug phase: prints "is debugging" under print_lock,
 * sleeps for debug_time ms.
 */
void	debug(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->print_lock);
	if (!is_simulation_running(coder->sim))
	{
		pthread_mutex_unlock(&coder->sim->print_lock);
		return ;
	}
	printf("%-5zu %-5d is debugging\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	if (!is_simulation_running(coder->sim))
		return ;
	msleep(coder->sim->debug_time);
}

/*
 * Refactor phase: prints "is refactoring" under print_lock,
 * sleeps for refactor_time ms.
 */
void	refactor(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->print_lock);
	if (!is_simulation_running(coder->sim))
	{
		pthread_mutex_unlock(&coder->sim->print_lock);
		return ;
	}
	printf("%-5zu %-5d is refactoring\n",
		get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);
	if (!is_simulation_running(coder->sim))
		return ;
	msleep(coder->sim->refactor_time);
}
