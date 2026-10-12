/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 13:23:53 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/12 17:33:06 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	ft_atol(const char *nptr)
{
	size_t	i;
	long	res;

	res = 0;
	i = 0;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		res = (res * 10) + (nptr[i] - '0');
		if (res > 2147483647)
			return (-1);
		i++;
	}
	return (res);
}

static int	parse_rules(int ac, char **av, t_rules *rules)
{
	(void)ac;
	rules->number_of_coders = ft_atol(av[1]);
	rules->time_to_burnout = ft_atol(av[2]);
	rules->time_to_compile = ft_atol(av[3]);
	rules->time_to_debug = ft_atol(av[4]);
	rules->time_to_refactor = ft_atol(av[5]);
	rules->number_of_compiles_required = ft_atol(av[6]);
	rules->dongle_cooldown = ft_atol(av[7]);
	if (rules->number_of_coders < 1 || rules->time_to_burnout < 1
		|| rules->time_to_compile < 0 || rules->time_to_debug < 0
		|| rules->time_to_refactor < 0
		|| rules->number_of_compiles_required < 1
		|| rules->dongle_cooldown < 0)
		return (1);
	return (0);
}

static int	is_digit_string(const char *string)
{
	int	i;

	if (!string || !string[0])
		return (1);
	i = 0;
	while (string[i])
	{
		if (string[i] < '0' || string[i] > '9')
			return (1);
		i++;
	}
	return (0);
}

static int	parse_scheduler(const char *str, t_rules *rules)
{
	if (strcmp(str, "fifo") == 0)
	{
		rules->scheduler = POLICY_FIFO;
		return (0);
	}
	if (strcmp(str, "edf") == 0)
	{
		rules->scheduler = POLICY_EDF;
		return (0);
	}
	return (1);
}

int	parser(int ac, char **av, t_rules *rules)
{
	int		i;

	(void)ac;
	i = 1;
	while (i < 8)
	{
		if (is_digit_string(av[i]) == 1)
			return (1);
		i++;
	}
	if (parse_rules(ac, av, rules) != 0)
		return (1);
	if (parse_scheduler(av[8], rules) != 0)
		return (1);
	return (0);
}
