#include "codexion.h"

void	destroy_all_rules_mutexes(t_rules *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_coders)
		pthread_mutex_destroy(&rules->dongles[i++]);
	pthread_mutex_destroy(&rules->print_mutex);
	pthread_mutex_destroy(&rules->state_mutex);
	pthread_cond_destroy(&rules->queue_cond);
}

void	free_all(t_rules *rules, t_coder *coders)
{
	free(coders);
	free(rules->dongles);
	free(rules->dongle_last_use);
	if (rules->priority_queue)
	{
		free(rules->priority_queue->nodes);
		free(rules->priority_queue);
	}
}

void	cleanup(t_rules *rules, t_coder **coders)
{
	int	i;
	
	i = 0;
	while (i < rules->number_of_coders)
		pthread_mutex_destroy(&(*coders)[i++].coder_mutex);
	destroy_all_rules_mutexes(rules);
	free_all(rules, *coders);
}