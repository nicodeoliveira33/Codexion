/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_sim.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 13:51:20 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/12 17:32:09 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_heap(t_rules *rules)
{
	int	count;

	count = rules->number_of_coders;
	rules->wait_queue = malloc(sizeof(t_heap));
	if (!rules->wait_queue)
		return (1);
	rules->wait_queue->coders = malloc(sizeof(t_coder *) * count);
	if (!rules->wait_queue->coders)
	{
		free(rules->wait_queue);
		rules->wait_queue = NULL;
		return (1);
	}
	rules->wait_queue->capacity = rules->number_of_coders;
	rules->wait_queue->size = 0;
	return (0);
}

static int	init_memory(t_rules *rules, t_coder **coders)
{
	rules->dongles = malloc(sizeof(t_dongle) * rules->number_of_coders);
	if (!rules->dongles)
		return (1);
	*coders = malloc(sizeof(t_coder) * rules->number_of_coders);
	if (!(*coders))
	{
		free(rules->dongles);
		rules->dongles = NULL;
		return (1);
	}
	memset(rules->dongles, 0, sizeof(t_dongle) * rules->number_of_coders);
	memset(*coders, 0, sizeof(t_coder) * rules->number_of_coders);
	if (init_heap(rules) != 0)
	{
		free(rules->dongles);
		free(*coders);
		rules->dongles = NULL;
		*coders = NULL;
		return (1);
	}
	return (0);
}

static int	init_mutexes_and_cond(t_rules *rules, t_coder *coders)
{
	int	i;

	if (pthread_mutex_init(&rules->global_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&rules->print_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&rules->global_mutex), 1);
	if (pthread_mutex_init(&rules->stop_sim_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&rules->global_mutex),
			pthread_mutex_destroy(&rules->print_mutex), 1);
	i = 0;
	while (i < rules->number_of_coders)
	{
		if (pthread_mutex_init(&coders[i].mutex, NULL) != 0)
			return (destroy_mutex_coders(coders, i),
				destroy_mutex_rules(rules), 1);
		if (pthread_cond_init(&coders[i].wake_up, NULL) != 0)
			return (pthread_mutex_destroy(&coders[i].mutex),
				destroy_mutex_coders(coders, i),
				destroy_mutex_rules(rules), 1);
		i++;
	}
	return (0);
}

static int	init_dongles_and_coders(t_rules *rules, t_coder *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		rules->dongles[i].id = i;
		rules->dongles[i].taken = false;
		rules->dongles[i].last_release = 0;
		i++;
	}
	i = 0;
	while (i < count)
	{
		coders[i].id = i + 1;
		coders[i].compiles_done = 0;
		coders[i].last_compile_start = 0;
		coders[i].left_dongle = &rules->dongles[i];
		coders[i].right_dongle = &rules->dongles[(i + 1) % count];
		coders[i].rules = rules;
		coders[i].request_order = 0;
		i++;
	}
	return (0);
}

int	init_sim(t_rules *rules, t_coder **coders)
{
	if (init_memory(rules, coders) != 0)
		return (free_all(rules, *coders), 1);
	if (init_mutexes_and_cond(rules, *coders) != 0)
		return (free_all(rules, *coders), 1);
	init_dongles_and_coders(rules, *coders, rules->number_of_coders);
	return (0);
}
