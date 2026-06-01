/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 00:00:00 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/01 15:10:59 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

static void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timeval	tv;
	struct timespec	ts;
	size_t			cooldown_end;

	pthread_mutex_lock(&dongle->dongle_lock);
	
	// Add to queue
	enqueue_coder(dongle, coder);

	while (1)
	{
		// If used or not my turn, just wait for a broadcast
		if (dongle->is_used || dongle->waiting[0] != coder)
		{
			pthread_cond_wait(&dongle->dongle_cond, &dongle->dongle_lock);
		}
		else
		{
			// It's my turn and not used. Has cooldown passed?
			cooldown_end = dongle->last_used + coder->sim->dongle_cooldown;
			if (get_time() >= cooldown_end)
				break; // Yes! we can take it.

			// Otherwise, wait until the cooldown expires
			gettimeofday(&tv, NULL);
			// Calculate exact target timespec
			ts.tv_sec = tv.tv_sec + ((cooldown_end - get_time()) / 1000);
			ts.tv_nsec = (tv.tv_usec * 1000) + (((cooldown_end - get_time()) % 1000) * 1000000);
			if (ts.tv_nsec >= 1000000000)
			{
				ts.tv_sec += 1;
				ts.tv_nsec -= 1000000000;
			}
			pthread_cond_timedwait(&dongle->dongle_cond, &dongle->dongle_lock, &ts);
		}
	}

	// Remove from queue and mark as used
	remove_coder(dongle, coder);
	dongle->is_used = 1;
	
	pthread_mutex_lock(&coder->sim->print_lock);
	printf("%zu %d has taken a dongle\n", get_time() - coder->sim->start_time, coder->id + 1);
	pthread_mutex_unlock(&coder->sim->print_lock);

	pthread_mutex_unlock(&dongle->dongle_lock);
}

static void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_lock);
	
	dongle->is_used = 0;
	dongle->last_used = get_time();
	
	// Wake up others waiting for this dongle
	pthread_cond_broadcast(&dongle->dongle_cond);
	
	pthread_mutex_unlock(&dongle->dongle_lock);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_dongle	*left_dongle;
	t_dongle	*right_dongle;
	unsigned int	i;

	coder = (t_coder *)arg;
	i = 0;
	
	// Deadlock prevention logic: Even IDs take left then right, Odd take right then left.
	if (coder->id % 2 == 0)
	{
		left_dongle = &coder->sim->dongles[coder->id];
		right_dongle = &coder->sim->dongles[(coder->id + 1) % coder->sim->nb_coders];
	}
	else
	{
		left_dongle = &coder->sim->dongles[(coder->id + 1) % coder->sim->nb_coders];
		right_dongle = &coder->sim->dongles[coder->id];
	}

	while (i < coder->sim->target_compiles)
	{
		// Acquire Dongles
		take_dongle(coder, left_dongle);
		take_dongle(coder, right_dongle);

		// Compile
		pthread_mutex_lock(&coder->sim->print_lock);
		printf("%zu %d is compiling\n", get_time() - coder->sim->start_time, coder->id + 1);
		pthread_mutex_unlock(&coder->sim->print_lock);
		
		pthread_mutex_lock(&coder->coder_lock);
		coder->last_compile_start = get_time();
		pthread_mutex_unlock(&coder->coder_lock);
		
		msleep(coder->sim->compile_time);
		
		// Release Dongles
		release_dongle(right_dongle);
		release_dongle(left_dongle);
		
		pthread_mutex_lock(&coder->coder_lock);
		coder->times_compiled++;
		pthread_mutex_unlock(&coder->coder_lock);

		// Debugging
		pthread_mutex_lock(&coder->sim->print_lock);
		printf("%zu %d is debugging\n", get_time() - coder->sim->start_time, coder->id + 1);
		pthread_mutex_unlock(&coder->sim->print_lock);
		
		msleep(coder->sim->debug_time);
		
		// Refactoring
		pthread_mutex_lock(&coder->sim->print_lock);
		printf("%zu %d is refactoring\n", get_time() - coder->sim->start_time, coder->id + 1);
		pthread_mutex_unlock(&coder->sim->print_lock);
		
		msleep(coder->sim->refactor_time);
		
		i++;
	}
	return (NULL);
}

int	start_simulation(t_sim *sim)
{
	int	i;

	sim->start_time = get_time();
	sim->is_running = 1;
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	if (!sim->coders)
		return (1);
	i = -1;
	while ((unsigned int)++i < sim->nb_coders)
	{
		sim->coders[i].id = (unsigned int)i;
		sim->coders[i].times_compiled = 0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].sim = sim;
		pthread_mutex_init(&sim->coders[i].coder_lock, NULL);
		pthread_create(&sim->coders[i].thread, NULL, coder_routine, &sim->coders[i]);
	}
	i = -1;
	while ((unsigned int)++i < sim->nb_coders)
		pthread_join(sim->coders[i].thread, NULL);
	return (0);
}
