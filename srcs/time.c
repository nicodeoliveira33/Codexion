/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 17:14:18 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/08 15:25:54 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void	ft_usleep(long time_in_ms, t_rules *rules)
{
	long	start;
	long	remaining;

	start = get_time();
	while ((get_time() - start) < time_in_ms)
	{
		if (is_simulation_stopped(rules))
			break ;
		remaining = time_in_ms - (get_time() - start);
		if (remaining > 100)
			usleep(100000);
		else
			usleep(remaining * 1000);
	}
}

void	convert_time(long timestamp, struct timespec *ts)
{
	ts->tv_sec = timestamp / 1000;
	ts->tv_nsec = (timestamp % 1000) * 1000000L;
}
