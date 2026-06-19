/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 11:22:05 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 17:12:55 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define NB_ARGS 9

/* ---------- FORWARD DECLARATIONS ----------*/
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;
typedef struct s_sim	t_sim;

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
	int				last_user;
	pthread_mutex_t	dongle_lock;
	pthread_cond_t	dongle_cond;
	size_t			last_used;
	t_coder			*waiting[2];
	int				queue_size;
}	t_dongle;

/* Simulation structure */
typedef struct s_sim
{
	t_dongle		*dongles;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	sim_lock;
	pthread_t		burnout_thread;
	t_scheduler		scheduler;
	unsigned int	nb_coders;
	unsigned int	burnout_time;
	unsigned int	compile_time;
	unsigned int	debug_time;
	unsigned int	refactor_time;
	unsigned int	target_compiles;
	unsigned int	dongle_cooldown;
	volatile int	is_running;
	size_t			start_time;
	t_coder			*coders;
}	t_sim;

/* coders structure */
typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				times_compiled;
	size_t			last_compile_start;
	int				is_waiting;
	t_sim			*sim;
	pthread_mutex_t	coder_lock;
}	t_coder;

/* ----------- PROTOTYPES ----------- */

// ----- parse.c -----
int			check_args(int argc, char **argv);
int			show_help(int helpId, int argc, char **argv);

// ----- time.c -----
size_t		get_time(void);
void		msleep(size_t time);
void		set_simulation_start_time(size_t start);
size_t		get_elapsed_time(void);

// ----- init.c -----
t_sim		*init_sim(char *argv[]);
t_dongle	*init_dongle(int nb_coders);
t_scheduler	init_scheduler(char *argv[]);
void		init_coders(t_sim *sim);
void		init_misc(t_sim *sim);

// ----- queue.c -----
void		enqueue_coder(t_dongle *dongle, t_coder *coder);
void		remove_coder(t_dongle *dongle, t_coder *coder);
void		release_dongle(t_dongle *dongle);

// ----- action.c -----
void		*coder_routine(void *arg);
void		routine_loop(t_coder *coder, t_dongle *donglel, t_dongle *dongler);
void		compile(t_coder *coder, t_dongle *l_dongle, t_dongle *r_dongle);
void		debug(t_coder *coder);
void		refactor(t_coder *coder);

// ----- routine.c -----
int			take_dongle(t_coder *coder, t_dongle *dongle);
int			take_dongle_timeout(t_coder *coder, t_dongle *dongle,
				size_t timeout_ms);

// ----- simulation.c -----
int			is_simulation_running(t_sim *sim);
int			start_simulation(t_sim *sim);
void		end_simulation(t_sim *sim, int coder_id, int has_printed);
void		cleanup_sim(t_sim *sim);

// ----- burnout.c -----
void		*burnout_monitor(void *arg);
int			has_coder_burned_out(t_coder *coder);
void		stop_and_broadcast(t_sim *sim);

// ---- print.c -----
void		log_action(char *message, int coder_id);
void		print_set_mutex(pthread_mutex_t *lock);
void		print_lock(int state);
void		log_dongle(int coder_id, size_t timestamp);

// ----- dongle.c -----
void		dongle_take_wait(t_dongle *dongle, t_coder *coder);
