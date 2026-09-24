#include "codexion.h"

void	print_status(t_coder *coder, char *status)
{
	long	timestamp;

	if (!is_simulation_stopped(coder->rules))
	{
		pthread_mutex_lock(&coder->rules->write_mutex);
		if (!is_simulation_stopped(coder->rules))
		{
			timestamp = get_time() - coder->rules->start_time;
			printf("%ld %d %s\n", timestamp, coder->id, status);
		}
		pthread_mutex_unlock(&coder->rules->write_mutex);
	}
}

void	print_burnout(t_coder *coder, char *str)
{
	long	timestamp;

	pthread_mutex_lock(&coder->rules->write_mutex);
	timestamp = get_time() - coder->rules->start_time;
	printf("%ld %d %s\n", timestamp, coder->id, str);
	pthread_mutex_unlock(&coder->rules->write_mutex);
}