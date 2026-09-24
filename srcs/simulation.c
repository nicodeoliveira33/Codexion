#include "codexion.h"


int is_simulation_stopped(t_rules *rules)
{
    int stopped;

    pthread_mutex_lock(&rules->stop_mutex);
    stopped = rules->stop_simulation;
    pthread_mutex_unlock(&rules->stop_mutex);
    
    return (stopped);
}

void	set_simulation_stop(t_rules *rules)
{
	int	i;

	pthread_mutex_lock(&rules->stop_mutex);
	rules->stop_simulation = 1;
	pthread_mutex_unlock(&rules->stop_mutex);
	
	i = 0;
	while (i < rules->number_of_coders)
	{
		pthread_mutex_lock(&rules->dongles[i].mutex);
		pthread_cond_broadcast(&rules->dongles[i].cond);
		pthread_mutex_unlock(&rules->dongles[i].mutex);
		i++;
	}
}

void	start_simulation(t_rules *rules, t_coder *coders)
{
	int	i;

	rules->start_time = get_time();
	i = 0;
	while (i < rules->number_of_coders)
	{
		pthread_mutex_lock(&coders[i].coder_mutex);
		coders[i].last_compile_start = rules->start_time;
		pthread_mutex_unlock(&coders[i].coder_mutex);
		i++;
	}
	i = 0;
	while (i < rules->number_of_coders)
	{
		pthread_create(&coders[i].thread, NULL, coder_routine, &coders[i]);
		i++;
	}
	pthread_create(&rules->monitor, NULL, monitor_routine, coders);
	i = 0;
	while (i < rules->number_of_coders)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
	pthread_join(rules->monitor, NULL);
}