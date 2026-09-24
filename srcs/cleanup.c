#include "codexion.h"

static void	free_dongles(t_rules *rules)
{
	int	i;

	if (!rules->dongles)
		return ;
	i = 0;
	while (i < rules->number_of_coders)
	{
		pthread_mutex_destroy(&rules->dongles[i].mutex);
		pthread_cond_destroy(&rules->dongles[i].cond);
		if (rules->dongles[i].queue.data)
			free(rules->dongles[i].queue.data);
		i++;
	}
	free(rules->dongles);
	rules->dongles = NULL;
}

static void	free_coders(t_rules *rules, t_coder *coders)
{
	int	i;

	if (!coders)
		return ;
	i = 0;
	while (i < rules->number_of_coders)
	{
		pthread_mutex_destroy(&coders[i].coder_mutex);
		i++;
	}
	free(coders);
}

void	cleanup(t_rules *rules, t_coder *coders)
{
	if (!rules)
		return ;
	free_coders(rules, coders);
	free_dongles(rules);
	pthread_mutex_destroy(&rules->write_mutex);
	pthread_mutex_destroy(&rules->stop_mutex);
}