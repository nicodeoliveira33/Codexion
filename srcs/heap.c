/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 15:32:15 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/08 20:08:59 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_heap(t_coder **parent, t_coder **current)
{
	t_coder	*temp;

	temp = *current;
	*current = *parent;
	*parent = temp;
}

static void	heapify_up(t_heap *heap, int current, t_scheduler scheduler)
{
	int		parent;
	t_coder	**coders;

	coders = heap->coders;
	while (current > 0)
	{
		parent = (current - 1) / 2;
		if (has_priority(coders[current], coders[parent], scheduler))
		{
			swap_heap(&coders[parent], &coders[current]);
			current = parent;
		}
		else
			break ;
	}
}

static void	heapify_down(t_heap *heap, int current, t_scheduler scheduler)
{
	int		left;
	int		right;
	int		smallest;
	t_coder	**coders;

	coders = heap->coders;
	while (1)
	{
		left = 2 * current + 1;
		right = 2 * current + 2;
		smallest = current;
		if (left < heap->size
			&& has_priority(coders[left], coders[smallest], scheduler))
			smallest = left;
		if (right < heap->size
			&& has_priority(coders[right], coders[smallest], scheduler))
			smallest = right;
		if (smallest == current)
			break ;
		swap_heap(&coders[current], &coders[smallest]);
		current = smallest;
	}
}

int	heap_insert(t_heap *heap, t_coder *coder, t_scheduler scheduler)
{
	int		current;
	t_coder	**coders;

	coders = heap->coders;
	if (heap->size >= heap->capacity)
		return (1);
	coders[heap->size] = coder;
	heap->size++;
	current = heap->size - 1;
	heapify_up(heap, current, scheduler);
	return (0);
}

void	heap_remove(t_heap *heap, t_coder *coder, t_scheduler scheduler)
{
	int		i;

	i = 0;
	while (i < heap->size && heap->coders[i] != coder)
		i++;
	if (i == heap->size)
		return ;
	heap->coders[i] = heap->coders[heap->size - 1];
	heap->size--;
	if (i < heap->size)
	{
		heapify_down(heap, i, scheduler);
		heapify_up(heap, i, scheduler);
	}
}
