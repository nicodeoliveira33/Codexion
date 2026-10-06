#include "codexion.h"

long	get_time(void)
{
	struct timespec	ts;

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		return (-1);
	return (ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

void	ft_usleep(long time_in_ms, t_rules *rules)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time_in_ms)
	{
		if (is_simulation_stopped(rules))
			break ;
		usleep(100);
	}
}

long	calculate_timeout(t_dongle *d1, t_dongle *d2, long now, long cooldown)
{
	long	timeout_d1;
	long	timeout_d2;
	long	timeout_ms;

	timeout_d1 = (d1->last_release + cooldown) - now;
	timeout_d2 = (d2->last_release + cooldown) - now;
	if (timeout_d1 < timeout_d2)
		timeout_ms = timeout_d1;
	else
		timeout_ms = timeout_d2;
	if (timeout_ms < 0)
		timeout_ms = 0;
	return (timeout_ms);
}
