/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 11:22:05 by tobesson          #+#    #+#             */
/*   Updated: 2026/05/19 15:48:33 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define NB_ARGS 9

// ----- parse.c -----
int		check_args(int argc, char **argv);
int		show_help(int helpId, int argc, char **argv);

// ----- time.c -----
size_t	get_time(void);
void	umimir(size_t time);
void	mimir(size_t time);

/* ---------- FORWARD DECLARATIONS ----------*/
typedef struct s_coder t_coder;
typedef struct s_dongle t_dongle;
typedef struct s_sim t_sim;

/* ---------- STRUCTS & UTILS ----------*/

/* Scheduler mode used when for arbitration queue. */
typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

/* dongle structure
contain a small heap of the two coders that can have acces to this dongle
sorted based on the scheduler
*/
typedef struct s_dongle
{
	int				id;
	int				is_used;
	pthread_mutex_t	dongle_lock;
	pthread_cond_t	dongle_cond;
	size_t			last_used;
	t_coder			*waiting;
}	t_dongle;

/* Simulation structure */
typedef struct s_sim
{
	t_dongle		*dongles;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	sim_lock;
	t_scheduler		scheduler;
	unsigned int	nb_coders;
	unsigned int	burnout_time;
	unsigned int	compile_time;
	unsigned int	debug_time;
	unsigned int	refactor_time;
	unsigned int	target_compiles;
	unsigned int	dongle_cooldown;
	int				is_running;
	size_t			start_time;
}	t_sim;

/* coders structure */
typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				times_compiled;
	size_t			last_compile_start;
	t_sim			*sim;
	pthread_mutex_t	coder_lock;
}	t_coder;

// ----- init.c -----
t_coder		*init_waitings(void);
t_dongle	*init_dongle(int nb_coders);
t_scheduler	init_scheduler(char *argv[]);
t_sim		*init_sim(char *argv[]);
