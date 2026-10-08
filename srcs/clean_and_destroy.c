/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_and_destroy.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 14:42:43 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/08 17:08:48 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_mutex_coders(t_coder *coders, int count)
{
	int	i;

	if (!coders)
		return ;
	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&coders[i].mutex);
		pthread_cond_destroy(&coders[i].wake_up);
		i++;
	}
}

void	destroy_mutex_rules(t_rules *rules)
{
	pthread_mutex_destroy(&rules->global_mutex);
	pthread_mutex_destroy(&rules->print_mutex);
	pthread_mutex_destroy(&rules->stop_sim_mutex);
}

void	free_all(t_rules *rules, t_coder *coders)
{
	if (coders)
		free(coders);
	if (rules->dongles)
		free(rules->dongles);
	if (rules->wait_queue)
		heap_destroy(rules->wait_queue);
}

void	cleanup(t_rules *rules, t_coder **coders)
{
	destroy_mutex_coders(*coders, rules->number_of_coders);
	destroy_mutex_rules(rules);
	free_all(rules, *coders);
}
