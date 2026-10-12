/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 17:52:05 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/12 17:32:02 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_destroy(t_heap *heap)
{
	if (!heap)
		return ;
	if (heap->coders)
		free(heap->coders);
	free(heap);
}

static long	get_sort_key(t_coder *coder, t_scheduler scheduler)
{
	if (scheduler == POLICY_EDF)
		return (coder->last_compile_start + coder->rules->time_to_burnout);
	else
		return (coder->request_order);
}

bool	has_priority(t_coder *c1, t_coder *c2, t_scheduler schedule)
{
	long	key1;
	long	key2;

	key1 = get_sort_key(c1, schedule);
	key2 = get_sort_key(c2, schedule);
	if (key1 == key2)
		return (c1->request_order < c2->request_order);
	return (key1 < key2);
}
