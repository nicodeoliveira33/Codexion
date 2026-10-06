#include "codexion.h"

bool	should_swap(t_heap_node parent, t_heap_node child)
{
	if (parent.key > child.key)
		return (true);
	if (parent.key == child.key && parent.id_tie > child.id_tie)
		return (true);
	return (false);
}

void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

long	calculate_key(t_coder *coder)
{
	long	key;

	pthread_mutex_lock(&coder->mutex);
	if (coder->rules->scheduler == POLICY_FIFO)
		key = get_time();
	else
		key = coder->last_compile_start + coder->rules->time_to_burnout;
	pthread_mutex_unlock(&coder->mutex);
	return (key);
}

void	heapify_down(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	smallest;

	while (true)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < heap->size
			&& should_swap(heap->nodes[smallest], heap->nodes[left]))
			smallest = left;
		if (right < heap->size
			&& should_swap(heap->nodes[smallest], heap->nodes[right]))
			smallest = right;
		if (smallest == i)
			break ;
		swap_nodes(&heap->nodes[i], &heap->nodes[smallest]);
		i = smallest;
	}
}
