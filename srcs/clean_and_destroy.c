#include "codexion.h"

void	destroy_dongles(t_dongle *dongle, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&dongle[i].mutex);
		free(dongle[i].queue->nodes);
		free(dongle[i].queue);
		i++;
	}
}

void	destroy_coders(t_coder *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&coders[i].mutex);
		pthread_cond_destroy(&coders[i].wake_up);
		i++;
	}
}

void	destroy_all_rules_mutexes(t_rules *rules)
{
	pthread_mutex_destroy(&rules->print_mutex);
	pthread_mutex_destroy(&rules->state_mutex);
}

void	free_all(t_rules *rules, t_coder *coders)
{
	free(coders);
	free(rules->dongles);
}

void	cleanup(t_rules *rules, t_coder **coders)
{
	destroy_coders(*coders, rules->number_of_coders);
	destroy_dongles(rules->dongles, rules->number_of_coders);
	destroy_all_rules_mutexes(rules);
	free_all(rules, *coders);
}
