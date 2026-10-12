/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 13:04:43 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/12 17:32:52 by ndias-de         ###   ########.fr       */
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
		fprintf(stderr, "\nInvalid Input!\n");
		print_invalid_args();
		return (1);
	}
	coders = NULL;
	if (init_sim(&rules, &coders) != 0)
	{
		fprintf(stderr, "Init error\n");
		return (1);
	}
	if (start_sim(&rules, coders) != 0)
	{
		fprintf(stderr, "Start sim error!\n");
		cleanup(&rules, &coders);
		return (1);
	}
	cleanup(&rules, &coders);
	return (0);
}
