#include "codexion.h"

t_heap	*init_heap(int capacity)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->nodes = malloc(sizeof(t_heap_node) * capacity);
	if (!heap->nodes)
	{
		free(heap);
		return (NULL);
	}
	heap->capacity = capacity;
	heap->size = 0;
	return (heap);
}

void	heap_insert(t_heap *heap, t_coder *coder, long key)
{
	int	i;

	if (!heap || heap->size >= heap->capacity)
		return ;
	i = heap->size;
	heap->nodes[i].coder = coder;
	heap->nodes[i].key = key;
	heap->nodes[i].id_tie = coder->id;
	heap->size++;
	while (i > 0 && should_swap(heap->nodes[(i - 1) / 2], heap->nodes[i]))
	{
		swap_nodes(&heap->nodes[(i - 1) / 2], &heap->nodes[i]);
		i = (i - 1) / 2;
	}
}

t_coder	*heap_extract_min(t_heap *heap)
{
	t_coder	*min_coder;

	if (!heap || heap->size <= 0)
		return (NULL);
	min_coder = heap->nodes[0].coder;
	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];
	heapify_down(heap, 0);
	return (min_coder);
}