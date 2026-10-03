#include "codexion.h"

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

void	print_invalid_args()
{
	printf("Mandatory arguments:\n");
	printf("1. number_of_coders (integers and positive)\n");
	printf("2. time_to_burnout (integers and positive)\n");
	printf("3. time_to_compile (integers and positive)\n");
	printf("4. time_to_debug (integers and positive)\n");
	printf("5. time_to_refactor (integers and positive)\n");
	printf("6. number_of_compiles_required (integers and positive)\n");
	printf("7. dongle_cooldown (integers and positive)\n");
	printf("8. scheduler (fifo or edf)\n");
	printf("Example: 4 800 150 150 150 5 10 fifo\n");
}