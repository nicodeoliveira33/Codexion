/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 13:27:46 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/08 20:31:01 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_status(t_coder *coder, char *status)
{
	long	timestamp;

	if (!is_simulation_stopped(coder->rules))
	{
		pthread_mutex_lock(&coder->rules->print_mutex);
		if (!is_simulation_stopped(coder->rules))
		{
			timestamp = get_time() - coder->rules->start_time;
			printf("%ld %d %s\n", timestamp, coder->id, status);
		}
		pthread_mutex_unlock(&coder->rules->print_mutex);
	}
}

void	print_burnout(t_coder *coder, char *str)
{
	long	timestamp;

	pthread_mutex_lock(&coder->rules->print_mutex);
	timestamp = get_time() - coder->rules->start_time;
	printf("%ld %d %s\n", timestamp, coder->id, str);
	pthread_mutex_unlock(&coder->rules->print_mutex);
}

void	print_invalid_args(void)
{
	fprintf(stderr, "\nMandatory arguments:\n");
	fprintf(stderr, "1. number_of_coders            (positive integer)\n");
	fprintf(stderr, "2. time_to_burnout             (positive integer)\n");
	fprintf(stderr, "3. time_to_compile             (non-negative integer)\n");
	fprintf(stderr, "4. time_to_debug               (non-negative integer)\n");
	fprintf(stderr, "5. time_to_refactor            (non-negative integer)\n");
	fprintf(stderr, "6. number_of_compiles_required (positive integer)\n");
	fprintf(stderr, "7. dongle_cooldown             (non-negative integer)\n");
	fprintf(stderr, "8. scheduler                   (fifo or edf)\n\n");
	fprintf(stderr, "Example: 4 800 150 150 150 5 10 fifo\n");
}
