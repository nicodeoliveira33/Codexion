/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 13:04:43 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/08 20:27:21 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char **av)
{
	t_rules	rules;
	t_coder	*coders;

	memset(&rules, 0, sizeof(t_rules));
	if (ac != 9 || parser(ac, av, &rules) == 1)
	{
		write(2, "\nInvalid Input!\n", 16);
		print_invalid_args();
		return (1);
	}
	coders = NULL;
	if (init_sim(&rules, &coders) != 0)
	{
		write(2, "Init error\n", 11);
		return (1);
	}
	if (start_sim(&rules, coders) != 0)
	{
		write(2, "Start sim error!\n", 17);
		cleanup(&rules, &coders);
		return (1);
	}
	cleanup(&rules, &coders);
	return (0);
}
