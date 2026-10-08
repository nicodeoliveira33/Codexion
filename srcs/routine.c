/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 15:53:00 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/08 16:42:29 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	handle_single_coder(t_rules *rules, t_coder *coder)
{
	coder->left_dongle->taken = true;
	print_status(coder, "has taken a dongle");
	ft_usleep(rules->time_to_burnout, rules);
	coder->left_dongle->taken = false;
}

static void	compile(t_rules *rules, t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_start = get_time();
	pthread_mutex_unlock(&coder->mutex);
	print_status(coder, "is compiling");
	ft_usleep(rules->time_to_compile, rules);
	pthread_mutex_lock(&coder->mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->mutex);
}

static void	debug(t_rules *rules, t_coder *coder)
{
	print_status(coder, "is debugging");
	ft_usleep(rules->time_to_debug, rules);
}

static void	refactor(t_rules *rules, t_coder *coder)
{
	print_status(coder, "is refactoring");
	ft_usleep(rules->time_to_refactor, rules);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_rules	*rules;

	coder = (t_coder *)arg;
	rules = coder->rules;
	if (rules->number_of_coders == 1)
	{
		handle_single_coder(rules, coder);
		return (NULL);
	}
	while (!is_simulation_stopped(rules))
	{
		if (take_dongles(rules, coder) != 0)
			return (NULL);
		compile(rules, coder);
		put_dongles(rules, coder);
		debug(rules, coder);
		refactor(rules, coder);
	}
	return (NULL);
}
