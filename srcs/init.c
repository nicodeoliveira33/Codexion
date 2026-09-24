#include "codexion.h"


static int	init_mutexes(t_rules *rules)
{
	rules->stop_simulation = 0;
	rules->start_time = 0;
	if (pthread_mutex_init(&rules->write_mutex, NULL) != 0)
		return (1);
	if	(pthread_mutex_init(&rules->stop_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&rules->write_mutex);
		return (1);
	}	
	return (0);
}

static int	init_dongles(t_rules *rules)
{
	int	i;

	rules->dongles = malloc(sizeof(t_dongle) * rules->number_of_coders);
	if (!rules->dongles)
		return (1);
	i = 0;
	while (i < rules->number_of_coders)
	{
		rules->dongles[i].id = i + 1;
		rules->dongles[i].taken = 0;
		rules->dongles[i].last_released = 0;
		rules->dongles[i].current_holder_id = 0;
		if (pthread_mutex_init(&rules->dongles[i].mutex, NULL) != 0)
			return (1);
		if (pthread_cond_init(&rules->dongles[i].cond, NULL) != 0)
			return (1);
		if (heap_init(&rules->dongles[i].queue, rules->number_of_coders) != 0)
			return (1);
		i++;
	}
	return (0);
}

static int init_coders(t_rules *rules, t_coder **coders)
{
	int	i;

	*coders = malloc (sizeof(t_coder) * rules->number_of_coders);
	if (!(*coders))
		return (1);
	i = 0;
	while (i < rules->number_of_coders)
	{
		(*coders)[i].id = i + 1;
		(*coders)[i].compiled = 0;
		(*coders)[i].last_compile_start = 0;
		(*coders)[i].rules = rules;
		(*coders)[i].left_dongle = &rules->dongles[i];
		(*coders)[i].right_dongle = &rules->dongles[(i + 1) % rules->number_of_coders];
		if (pthread_mutex_init(&(*coders)[i].coder_mutex, NULL) != 0)
			return (1);
		i++;
	}
	return (0);
}

int	init_simulation(t_rules *rules, t_coder **coders)
{
	if (init_mutexes(rules) != 0 ||
		init_dongles(rules) != 0 ||
		init_coders(rules, coders) != 0)
		return (1);
	return (0);
}