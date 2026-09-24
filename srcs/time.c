#include "codexion.h"


long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void	ft_usleep(long time_in_ms, t_rules *rules)
{
	long start;
	
	start = get_time();
	while ((get_time() - start) < time_in_ms)
	{
		if (is_simulation_stopped(rules))
			break;
		usleep(500);
	}
}