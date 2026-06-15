/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tobesson <tobesson@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 12:05:39 by tobesson          #+#    #+#             */
/*   Updated: 2026/06/15 12:16:53 by tobesson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc.h"

void	log_term(char *message, t_sim *sim)
{
	pthread_mutex_lock(&sim->print_lock);
	printf("%s\n", message);
	pthread_mutex_unlock(&sim->print_lock);
}
