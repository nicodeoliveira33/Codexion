#include "codexion.h"

static int	init_memory(t_rules *rules, t_coder **coders)
{
	*coders = malloc (sizeof(t_coder) * rules->number_of_coders);
	if (!(*coders))
		return (1);
	rules->dongles = malloc (sizeof(t_dongle) * rules->number_of_coders);
	if (!rules->dongles)
		return (free(*coders), 1);
	memset(*coders, 0, sizeof(t_coder) * rules->number_of_coders);
	memset(rules->dongles, 0, sizeof(t_dongle) * rules->number_of_coders);
	return (0);
}

static int	init_mutexes(t_rules *rules)
{
	if (pthread_mutex_init(&rules->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&rules->state_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&rules->print_mutex), 1);
	return (0);
}

static int	init_dongles(t_rules *rules, t_dongle *dongle)
{
	int	i;

	i = 0;
	while (i < rules->number_of_coders)
	{
		dongle[i].id = i;
		dongle[i].taken = false;
		dongle[i].last_release = 0;
		dongle[i].sequence_counter = 0;
		if (pthread_mutex_init(&dongle[i].mutex, NULL) != 0)
			return (destroy_dongles(dongle, i), 1);
		dongle[i].queue = init_heap(rules->number_of_coders * 3);
		if (!dongle[i].queue)
			return (pthread_mutex_destroy(&dongle[i].mutex),
				destroy_dongles(dongle, i), 1);
		i++;
	}
	return (0);
}

static int	init_coders(t_rules *rules, t_coder *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		coders[i].id = i + 1;
		coders[i].compiles_done = 0;
		coders[i].last_compile_start = 0;
		coders[i].left_dongle = &rules->dongles[i];
		coders[i].right_dongle = &rules->dongles[(i + 1) % count];
		coders[i].rules = rules;
		coders[i].notified = false;
		if (pthread_mutex_init(&coders[i].mutex, NULL) != 0)
			return (destroy_coders(coders, i), 1);
		if (pthread_cond_init(&coders[i].wake_up, NULL) != 0)
			return (pthread_mutex_destroy(&coders[i].mutex),
				destroy_coders(coders, i), 1);
		i++;
	}
	return (0);
}

int	init_simulation(t_rules *rules, t_coder **coders)
{
	if (init_memory(rules, coders) != 0)
		return (1);
	if (init_mutexes(rules) != 0)
		return (free_all(rules, *coders), 1);
	if (init_dongles(rules, rules->dongles) != 0)
		return (destroy_all_rules_mutexes(rules),
			free_all(rules, *coders), 1);
	if (init_coders(rules, *coders, rules->number_of_coders) != 0)
		return (destroy_dongles(rules->dongles,
				rules->number_of_coders),
			destroy_all_rules_mutexes(rules),
			free_all(rules, *coders), 1);
	return (0);
}
