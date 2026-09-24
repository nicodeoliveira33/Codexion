#include "codexion.h"

void	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->coder_mutex);
	coder->last_compile_start = get_time();
	coder->compiled++;
	pthread_mutex_unlock(&coder->coder_mutex);
	print_status(coder, "is compiling");
	ft_usleep(coder->rules->time_to_compile, coder->rules);
}

void	debug(t_coder *coder)
{
	print_status(coder, "is debugging");
	ft_usleep(coder->rules->time_to_debug, coder->rules);
}

void	refactor(t_coder *coder)
{
	print_status(coder, "is refactoring");
	ft_usleep(coder->rules->time_to_refactor, coder->rules);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (is_simulation_stopped(coder->rules) == 0)
	{
		if (take_dongles(coder) != 0)
			break;
		compile(coder);
		put_dongles(coder);
		debug(coder);
		refactor(coder);
	}
	return NULL;
}