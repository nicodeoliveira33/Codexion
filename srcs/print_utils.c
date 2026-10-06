#include "codexion.h"

void	print_dongles(t_coder *coder, char *status)
{
	long	timestamp;

	if (!is_simulation_stopped(coder->rules))
	{
		pthread_mutex_lock(&coder->rules->print_mutex);
		if (!is_simulation_stopped(coder->rules))
		{
			timestamp = get_time() - coder->rules->start_time;
			printf("%ld %d %s\n", timestamp, coder->id, status);
			timestamp = get_time() - coder->rules->start_time;
			printf("%ld %d %s\n", timestamp, coder->id, status);
		}
		pthread_mutex_unlock(&coder->rules->print_mutex);
	}
}

void	print_status(t_coder *coder, char *status)
{
	long	timestamp;

	if (!is_simulation_stopped(coder->rules))
	{
		pthread_mutex_lock(&coder->rules->print_mutex);
		if (!is_simulation_stopped(coder->rules))
		{
			timestamp = get_time() - coder->rules->start_time;
			printf("%ld %d %s\n", timestamp, coder->id, status);
		}
		pthread_mutex_unlock(&coder->rules->print_mutex);
	}
}

void	print_burnout(t_coder *coder, char *str)
{
	long	timestamp;

	pthread_mutex_lock(&coder->rules->print_mutex);
	timestamp = get_time() - coder->rules->start_time;
	printf("%ld %d %s\n", timestamp, coder->id, str);
	pthread_mutex_unlock(&coder->rules->print_mutex);
}

void	print_invalid_args(void)
{
	printf("\nMandatory arguments:\n");
	printf("1. number_of_coders            (positive integers)\n");
	printf("2. time_to_burnout             (positive integers)\n");
	printf("3. time_to_compile             (positive integers)\n");
	printf("4. time_to_debug               (positive integers)\n");
	printf("5. time_to_refactor            (positive integers)\n");
	printf("6. number_of_compiles_required (positive integers)\n");
	printf("7. dongle_cooldown             (positive integers)\n");
	printf("8. scheduler                   (fifo or edf)\n\n");
	printf("Example: 4 800 150 150 150 5 10 fifo\n");
}
