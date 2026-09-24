#include "codexion.h"

int	heap_init(t_heap *heap, int capacity)
{
	heap->data = malloc(sizeof(t_request) * capacity);
	if (!heap->data)
		return (1);
	heap->size = 0;
	heap->capacity = capacity;
	return (0);
}

void	heap_destroy(t_heap *heap)
{
	if (heap->data)
		free(heap->data);
	heap->data = NULL;
	heap->size = 0;
}

static int	has_higher_priority(t_request a, t_request b, t_sched policy)
{
	if (policy == POLICY_FIFO)
	{
		if (a.arrival_time != b.arrival_time)
			return (a.arrival_time < b.arrival_time);
		return (a.coder_id < b.coder_id);
	}
	if (a.deadline != b.deadline)
		return (a.deadline < b.deadline);
	return (a.coder_id < b.coder_id);
}

void	heap_insert(t_heap *heap, t_request req, t_sched policy)
{
	int			i;
	int			parent;
	t_request	tmp;

	if (heap->size >= heap->capacity)
		return ;
	i = heap->size;
	heap->data[i] = req;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (has_higher_priority(heap->data[i], heap->data[parent], policy))
		{
			tmp = heap->data[i];
			heap->data[i] = heap->data[parent];
			heap->data[parent] = tmp;
			i = parent;
		}
		else
			break ;
	}
}

t_request	heap_extract_min(t_heap *heap, t_sched policy)
{
	t_request	min_req;
	int			i;
	int			smallest;
	t_request	tmp;

	if (heap->size == 0)
		return ((t_request){0, 0, 0});
	min_req = heap->data[0];
	heap->data[0] = heap->data[heap->size - 1];
	heap->size--;
	i = 0;
	while (1)
	{
		smallest = i;
		if (2 * i + 1 < heap->size && has_higher_priority(heap->data[2 * i + 1], heap->data[smallest], policy))
			smallest = 2 * i + 1;
		if (2 * i + 2 < heap->size && has_higher_priority(heap->data[2 * i + 2], heap->data[smallest], policy))
			smallest = 2 * i + 2;
		if (smallest == i)
			break ;
		tmp = heap->data[i];
		heap->data[i] = heap->data[smallest];
		heap->data[smallest] = tmp;
		i = smallest;
	}
	return (min_req);
}