#include "codexion.h"

static void	compile(t_coder *coders)
{
	pthread_mutex_lock(&coders->coder_mutex);
	coders->last_compile_start = get_time();
	coders->compiles_done++;
	pthread_mutex_unlock(&coders->coder_mutex);
	print_status(coders, "is compiling");
	ft_usleep(coders->rules->time_to_compile, coders->rules);
}

static void	debug(t_coder *coders)
{
	print_status(coders, "is debugging");
	ft_usleep(coders->rules->time_to_debug, coders->rules);
}

static void	refactor(t_coder *coders)
{
	print_status(coders, "is refactoring");
	ft_usleep(coders->rules->time_to_refactor, coders->rules);
}
void    *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;

	if (coder->rules->number_of_coders == 1)
	{
		take_dongle(coder);
		ft_usleep(coder->rules->time_to_burnout, coder->rules);
		return (NULL);
	}
	if (coder->id % 2 == 0)
		usleep(1000);
    while (!is_simulation_stopped(coder->rules))
    {
		take_dongles(coder);
		compile(coder);
		put_dongles(coder);
		debug(coder);
		refactor(coder);
    }
	return (NULL);
}