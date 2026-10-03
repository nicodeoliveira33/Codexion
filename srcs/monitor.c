#include "codexion.h"

static bool	check_burnout(t_coder *coder)
{
	long	now;
	long	last_compile;

	now = get_time();
	pthread_mutex_lock(&coder->coder_mutex);
	last_compile = coder->last_compile_start;
	pthread_mutex_unlock(&coder->coder_mutex);
	if ((now - last_compile) >= coder->rules->time_to_burnout)
	{
		set_simulation_stop(coder->rules);
		print_burnout(coder, "burned out");
		return (true);
	}
	return (false);
}

static bool	check_compiles(t_rules *rules, t_coder *coders)
{
	int	i;
	int	finished;

	if (rules->number_of_compiles_required == -1)
		return (false);
	i = 0;
	finished = 0;
	while (i < rules->number_of_coders)
	{
		pthread_mutex_lock(&coders[i].coder_mutex);
		if (coders[i].compiles_done >= rules->number_of_compiles_required)
			finished++;
		pthread_mutex_unlock(&coders[i].coder_mutex);
		i++;
	}
	if (finished >= rules->number_of_coders)
	{
		set_simulation_stop(rules);
		return (true);
	}
	return (false);
}

void	*monitor_routine(void *arg)
{
	t_coder	*coders;
	t_rules	*rules;
	int		i;

	coders = (t_coder *)arg;
	rules = coders[0].rules;
	usleep(1000);
	while (!is_simulation_stopped(rules))
	{
		i = 0;
		while (i < rules->number_of_coders)
		{
			if (check_burnout(&coders[i]))
				return (NULL);
			i++;
		}
		if (check_compiles(rules, coders))
			return (NULL);
		usleep(500);
	}
	return (NULL);
}