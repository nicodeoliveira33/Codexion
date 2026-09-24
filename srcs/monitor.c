#include "codexion.h"

static int	check_coder_burnout(t_coder *coder)
{
	long	now;

	pthread_mutex_lock(&coder->coder_mutex);
	now = get_time();
	if (now - coder->last_compile_start >= coder->rules->time_to_burnout)
	{
		set_simulation_stop(coder->rules);
		pthread_mutex_unlock(&coder->coder_mutex);
		print_burnout(coder, "burned out");
		return (1);
	}
	pthread_mutex_unlock(&coder->coder_mutex);
	return (0);
}

static int	check_all_compiled(t_coder *coders, t_rules *rules)
{
	int	i;
	int	finished;

	i = 0;
	finished = 0;
	while (i < rules->number_of_coders)
	{
		pthread_mutex_lock(&coders[i].coder_mutex);
		if (coders[i].compiled >= rules->number_of_compiles_required)
			finished++;
		pthread_mutex_unlock(&coders[i].coder_mutex);
		i++;
	}
	if (finished == rules->number_of_coders)
	{
		set_simulation_stop(rules);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_coder	*coders;
	t_rules	*rules;
	int		i;

	coders = (t_coder *)arg;
	rules = coders[0].rules;
	while (!is_simulation_stopped(rules))
	{
		i = 0;
		while (i < rules->number_of_coders)
		{
			if (check_coder_burnout(&coders[i]))
				return (NULL);
			i++;
		}
		if (check_all_compiled(coders, rules))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}