/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 11:20:59 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 15:46:10 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Thread entry point for each coder. Applies adaptive startup stagger
 * (even-first for even coder counts, odd-first for odd counts),
 * resolves the coder's left/right dongle pair using alternating ordering
 * to prevent deadlock, then enters the compile-debug-refactor loop.
 */
void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_dongle	*donglel;
	t_dongle	*dongler;

	coder = (t_coder *)arg;
	if (coder->sim->nb_coders % 2 == 0 && coder->id % 2 != 0)
		usleep(500);
	else if (coder->sim->nb_coders % 2 != 0 && coder->id % 2 == 0)
		usleep(500);
	if (coder->id % 2 == 0)
	{
		donglel = &coder->sim->dongles[coder->id];
		dongler = &coder->sim->dongles[(coder->id + 1)
			% coder->sim->nb_coders];
	}
	else
	{
		donglel = &coder->sim->dongles[(coder->id + 1)
			% coder->sim->nb_coders];
		dongler = &coder->sim->dongles[coder->id];
	}
	routine_loop(coder, donglel, dongler);
	return (NULL);
}

/*
 * Main coder loop: repeats compile -> debug -> refactor until the
 * required number of compiles is reached or the simulation stops.
 */
void	routine_loop(t_coder *coder, t_dongle *donglel, t_dongle *dongler)
{
	int	i;

	i = -1;
	while ((unsigned int)++i < coder->sim->target_compiles
		&& is_simulation_running(coder->sim))
	{
		compile(coder, donglel, dongler);
		if (!is_simulation_running(coder->sim))
			break ;
		debug(coder);
		if (!is_simulation_running(coder->sim))
			break ;
		refactor(coder);
	}
}

/*
 * Entry point: validates arguments, allocates the simulation,
 * runs it, cleans up and frees all resources.
 */
int	main(int ac, char **av)
{
	t_sim	*sim;

	if (check_args(ac, av) != 0)
		return (1);
	sim = init_sim(av);
	if (!sim)
		return (1);
	printf("\e[0;32mArguments are valid! Starting simulation...\e[0;37m\n");
	start_simulation(sim);
	cleanup_sim(sim);
	free(sim->coders);
	free(sim->dongles);
	free(sim);
	return (0);
}
