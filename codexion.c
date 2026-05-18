/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 11:20:59 by tobesson          #+#    #+#             */
/*   Updated: 2026/05/07 16:37:58 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders/inc.h"

int	main(int ac, char **av)
{
	if (check_args(ac, av) != 0)
		return (1);
	printf("\e[0;32mArguments are valid!\e[0;37m\n");
	return (0);
}