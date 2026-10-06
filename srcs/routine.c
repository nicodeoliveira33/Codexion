#include "codexion.h"

static void	handle_single_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->taken = true;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	print_status(coder, "has taken a dongle");
	usleep(coder->rules->time_to_burnout * 1000);
	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->taken = false;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
}

static void	compile(t_coder *coders)
{
	pthread_mutex_lock(&coders->mutex);
	coders->last_compile_start = get_time();
	coders->compiles_done++;
	pthread_mutex_unlock(&coders->mutex);
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

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->rules->number_of_coders == 1)
	{
		handle_single_coder(coder);
		return (NULL);
	}
	while (!is_simulation_stopped(coder->rules))
	{
		if (take_dongles(coder) != 0)
			break ;
		compile(coder);
		put_dongles(coder);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}
