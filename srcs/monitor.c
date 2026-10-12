/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 16:51:44 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/12 17:33:02 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	check_burnout(t_coder *coder)
{
	long	now;
	long	last_compile;

	now = get_time();
	pthread_mutex_lock(&coder->mutex);
	last_compile = coder->last_compile_start;
	pthread_mutex_unlock(&coder->mutex);
	if ((now - last_compile) >= coder->rules->time_to_burnout)
		return (true);
	return (false);
}

static bool	check_compiles(t_rules *rules, t_coder *coders)
{
	int	i;
	int	finished;

	i = 0;
	finished = 0;
	while (i < rules->number_of_coders)
	{
		pthread_mutex_lock(&coders[i].mutex);
		if (coders[i].compiles_done >= rules->number_of_compiles_required)
			finished++;
		pthread_mutex_unlock(&coders[i].mutex);
		i++;
	}
	if (finished >= rules->number_of_coders)
	{
		set_simulation_stop(rules, coders);
		return (true);
	}
	return (false);
}

void	*monitor_routine(void *arg)
{
	t_coder	*coders;
	t_rules	*rules;
	int		i;

	coders = (t_coder *)arg;
	rules = coders[0].rules;
	while (!is_simulation_stopped(rules))
	{
		i = 0;
		while (i < rules->number_of_coders)
		{
			if (check_burnout(&coders[i]))
			{
				set_simulation_stop(rules, coders);
				print_burnout(&coders[i], "burned out");
				return (NULL);
			}
			i++;
		}
		if (check_compiles(rules, coders))
			return (NULL);
		usleep(100);
	}
	return (NULL);
}
