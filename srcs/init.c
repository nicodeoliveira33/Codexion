#include "codexion.h"

static int	init_memory(t_rules *rules, t_coder **coders)
{
	*coders = malloc (sizeof(t_coder) * rules->number_of_coders);
	if (!(*coders))
		return (1);

	rules->dongles = malloc (sizeof(pthread_mutex_t) * rules->number_of_coders);
	if (!rules->dongles)
		return (free(*coders), 1);

	rules->dongle_last_use = malloc (sizeof(long) * rules->number_of_coders);
	if (!rules->dongle_last_use)
		return (free(*coders), free(rules->dongles), 1);

	memset(*coders, 0, sizeof(t_coder) * rules->number_of_coders);
	memset(rules->dongles, 0, sizeof(pthread_mutex_t) * rules->number_of_coders);
	memset(rules->dongle_last_use, 0, sizeof(long) * rules->number_of_coders);
	return (0);
}

static int	init_mutexes(t_rules *rules)
{
	int	i;

	if (pthread_mutex_init(&rules->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&rules->state_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&rules->print_mutex), 1);
	if (pthread_cond_init(&rules->queue_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&rules->print_mutex);
		pthread_mutex_destroy(&rules->state_mutex);
		return (1);
	}
	i = 0;
	while (i < rules->number_of_coders)
	{
		if (pthread_mutex_init(&rules->dongles[i], NULL) != 0)
			return (1);
		i++;
	}
	return (0);
}

static int	init_coders(t_rules *rules, t_coder **coders)
{
	int	i;

	i = 0;
	while (i < rules->number_of_coders)
	{
		(*coders)[i].id = i + 1;
    	(*coders)[i].compiles_done = 0;
    	(*coders)[i].last_compile_start = 0;
		(*coders)[i].left_dongle_idx = i;
    	(*coders)[i].right_dongle_idx = (i + 1) % rules->number_of_coders;
		(*coders)[i].rules = rules;
		if (pthread_mutex_init(&(*coders)[i].coder_mutex, NULL) != 0)
		{
			while (i > 0)
				pthread_mutex_destroy(&(*coders)[--i].coder_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

int	init_simulation(t_rules *rules, t_coder **coders)
{
	if (init_memory(rules, coders) != 0)
		return (1);
		
	rules->priority_queue = init_heap(rules->number_of_coders);
	if (!rules->priority_queue)
		return (free_all(rules, *coders), 1);
 
	if (init_mutexes(rules) != 0)
		return (free_all(rules, *coders), 1);

	if (init_coders(rules, coders) != 0)
		return (destroy_all_rules_mutexes(rules), free_all(rules, *coders), 1);

	return (0);
}