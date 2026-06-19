/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 11:20:59 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/19 19:03:30 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
 * current state of the project and remaing thing to fix before submission
 * TODO: Feat: Add a help message for the user when arguments are invalid
 * TODO: For the parsing, allow -0 for compatible argument/ allow +0 and +xxx positives numbers
 * TODO: Check all malloc return values and handle errors (actually the code crash if NULL is returned)
 * TODO: Check all pthread_create return values, protect all pthread_mutex_lock and pthread_mutex_unlock with error handling
 * also protect pthread_cond_wait and pthread_cond_signal with error handling (+ pthread_cond_broadcast and pthread_cond_destroy, pthread_join)
 * TODO: Small bug in output terminal: whn coder burn out, some other coder may still write a bit after the message "has burned out"
 * make sure to stop all coders before printing the burnout message
 * TODO: run tests with normiette, valgrind, helgrind, drd, valgrind breaker (in sgoinfre)
*/


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
	init_misc(sim);
	printf("\e[0;32mArguments are valid! Starting simulation...\e[0;37m\n");
	start_simulation(sim);
	cleanup_sim(sim);
	free(sim->coders);
	free(sim->dongles);
	free(sim);
	return (0);
}
