/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 11:20:59 by tobesson          #+#    #+#             */
/*   Updated: 2026/04/20 15:27:16 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/inc.h"



void* foo(void* arg) {
	int i;

	(void)arg;

	i = -1;
	pthread_t thisThread = pthread_self();
	printf("New thread, ID: %lu\n", (unsigned long)thisThread);
	while (1)
	{
		i++;
		printf("waiting %ds...\n", i);
		sleep(1);
	}
    return NULL;
}


int	tempo(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	pthread_t thread1;
	pthread_t thread2;
    pthread_create(&thread1, NULL, foo, NULL);
	sleep(2);
    pthread_create(&thread2, NULL, foo, NULL);
	sleep(5);
	pthread_cancel(thread1);
	pthread_cancel(thread2);
    // Wait for thread to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
	printf("Thread finished\n");
    return 0;
}


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