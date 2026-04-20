/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 11:20:59 by tobesson          #+#    #+#             */
/*   Updated: 2026/04/20 15:46:06 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/inc.h"


int checkRange(char *arg)
{
	int nb;

	nb = atoi(arg);
	if (nb < 0)
		return (1);
	return (0);
}

int validateArgs(int ac, char **av)
{
	int i;

	//check numbers of arguments and the validity of them
	if (ac != 9)
	{
		printf("Invalid number of argument\n");
		return (1);
	}
	i = 0;
	while (++i < 9)
	{
		if (checkRange(av[i]) && i != 8)
		{
			printf("Argument %d (%s) is invalid (must be >= 0)\n", i, av[i]);
			return (1);
		}
		else if (i == 8 && strcmp(av[i], "fifo") != 0 && strcmp(av[i], "edf") != 0)
		{
			printf("Invalid shceduler: need fifo or edf, got %s\n", av[i]);
			return (1);
		}
		
	}
	return (0);
}


int	main(int argc, char **argv)
{
	if (validateArgs(argc, argv))
		return (1);
	return (0);
}