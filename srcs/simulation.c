/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 18:58:51 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/08 18:48:56 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

bool	is_simulation_stopped(t_rules *rules)
{
	pthread_mutex_lock(&rules->stop_sim_mutex);
	if (rules->sim_stop)
	{
		pthread_mutex_unlock(&rules->stop_sim_mutex);
		return (true);
	}
	pthread_mutex_unlock(&rules->stop_sim_mutex);
	return (false);
}

void	set_simulation_stop(t_rules *rules, t_coder *coders)
{
	int	i;

	pthread_mutex_lock(&rules->stop_sim_mutex);
	rules->sim_stop = true;
	pthread_mutex_unlock(&rules->stop_sim_mutex);
	i = 0;
	pthread_mutex_lock(&rules->global_mutex);
	while (i < rules->number_of_coders)
	{
		pthread_cond_signal(&coders[i].wake_up);
		i++;
	}
	pthread_mutex_unlock(&rules->global_mutex);
}

static int	create_threads(t_rules *rules, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < rules->number_of_coders)
	{
		if (pthread_create(&coders[i].thread, NULL,
				coder_routine, &coders[i]) != 0)
			return (i);
		i++;
	}
	if (pthread_create(&rules->monitor, NULL, monitor_routine, coders) != 0)
		return (i);
	i++;
	return (i);
}

static void	join_threads(t_rules *rules, t_coder *coders, int count)
{
	int	i;

	i = 0;
	while (i < count && i < rules->number_of_coders)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
	if (count == rules->number_of_coders + 1)
		pthread_join(rules->monitor, NULL);
}

int	start_sim(t_rules *rules, t_coder *coders)
{
	int	i;
	int	j;

	rules->start_time = get_time();
	i = 0;
	while (i < rules->number_of_coders)
	{
		coders[i].last_compile_start = rules->start_time;
		i++;
	}
	j = create_threads(rules, coders);
	if (j < rules->number_of_coders + 1)
		return (set_simulation_stop(rules, coders),
			join_threads(rules, coders, j), 1);
	join_threads(rules, coders, j);
	return (0);
}
