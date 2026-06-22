/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 11:20:59 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/22 17:19:39 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * Entry point: validates arguments, allocates the simulation,
 * runs it, cleans up and frees all resources.
 */
int	main(int ac, char **av)
{
	t_sim	*sim;
	int		ret;

	if (check_args(ac, av) != 0)
		return (1);
	sim = init_sim(av);
	if (!sim)
		return (1);
	init_misc(sim);
	printf("\e[0;32mArguments are valid! Starting simulation...\e[0;37m\n");
	ret = start_simulation(sim);
	cleanup_sim(sim);
	free(sim->coders);
	free(sim->dongles);
	free(sim);
	return (ret);
}
