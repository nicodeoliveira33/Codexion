#include "codexion.h"

static int	create_threads(t_rules *rules, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < rules->number_of_coders)
	{
		if (pthread_create(&coders[i].thread, NULL,
				coder_routine, &coders[i]) != 0)
			return (i);
		i++;
	}
	if (pthread_create(&rules->monitor, NULL, monitor_routine, coders) != 0)
		return (i);
	return (i);
}

static void	join_threads(t_rules *rules, t_coder *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
	if (rules->monitor != 0)
		pthread_join(rules->monitor, NULL);
}

bool	is_simulation_stopped(t_rules *rules)
{
	bool	status;

	pthread_mutex_lock(&rules->state_mutex);
	status = rules->sim_stop;
	pthread_mutex_unlock(&rules->state_mutex);
	return (status);
}

void	set_simulation_stop(t_rules *rules, t_coder *coders)
{
	int	i;

	pthread_mutex_lock(&rules->state_mutex);
	rules->sim_stop = true;
	i = 0;
	while (i < rules->number_of_coders)
	{
		pthread_cond_signal(&coders[i].wake_up);
		i++;
	}
	pthread_mutex_unlock(&rules->state_mutex);
}

int	start_simulation(t_rules *rules, t_coder *coders)
{
	int	i;
	int	j;

	rules->start_time = get_time();
	rules->monitor = 0;
	i = 0;
	while (i < rules->number_of_coders)
	{
		pthread_mutex_lock(&coders[i].mutex);
		coders[i].last_compile_start = rules->start_time;
		pthread_mutex_unlock(&coders[i].mutex);
		i++;
	}
	j = create_threads(rules, coders);
	if (j < rules->number_of_coders || rules->monitor == 0)
		return (join_threads(rules, coders, j), 1);
	join_threads(rules, coders, j);
	return (0);
}
