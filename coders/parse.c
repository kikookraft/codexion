/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 14:54:04 by tobesson          #+#    #+#             */
/*   Updated: 2026/05/15 15:09:23 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

/*
check for all numbers if they are positive
and check if scheduler is fifo or edf
on fail show a message explaining what is wrong
return 1 when arguments are invalid (else 0)
*/
int	check_args(int argc, char **argv)
{
	int	i;
	int	val;

	if (argc != NB_ARGS)
		return (show_help(0, argc, argv));
	i = 0;
	while (++i < argc - 1)
	{
		val = atoi(argv[i]);
		if (val < 0 || (val <= 0 && i == 1))
			return (show_help(i, argc, argv));
	}
	if (strcmp(argv[i], "fifo") && strcmp(argv[i], "edf"))
		return (show_help(i, argc, argv));
	return (0);
}

/*
Show usefull help based on what is invalidated on he funtion above
always return 1 (error and exit)
*/
static const char	*get_error_name(int id)
{
	if (id == 1)
		return ("Number of coders");
	if (id == 2)
		return ("Time to burnout");
	if (id == 3)
		return ("Time to compile");
	if (id == 4)
		return ("Time to debug");
	if (id == 5)
		return ("Time to refactor");
	if (id == 6)
		return ("Number of compiles required");
	if (id == 7)
		return ("Dongle cooldown");
	return (NULL);
}

/*
Print the correct error message based on what is invalidated
*/
int	show_help(int helpId, int argc, char **argv)
{
	if (helpId == 0)
	{
		printf("\e[0;31mWrong number of arguments: ");
		printf("expected %d arguments, got %d!\e[0;37m\n",
			NB_ARGS - 1, argc - 1);
	}
	else if (helpId >= 1 && helpId <= 7)
	{
		printf("\e[0;31mInvalid value for %s!",
			get_error_name(helpId));
		printf(" need a positive integer, got %s!\e[0;37m\n",
			argv[helpId]);
	}
	else if (helpId == 8)
	{
		printf("\e[0;31mInvalid value for Scheduler!");
		printf(" need 'fifo' or 'edf', got %s!\e[0;37m\n",
			argv[helpId]);
	}
	return (1);
}
