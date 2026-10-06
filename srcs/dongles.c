#include "codexion.h"

static void	order_dongles(t_coder *coder, t_dongle **d1, t_dongle **d2)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		*d1 = coder->left_dongle;
		*d2 = coder->right_dongle;
	}
	else
	{
		*d1 = coder->right_dongle;
		*d2 = coder->left_dongle;
	}
}

static bool	can_acquire(t_coder *coder, t_dongle *d1,
	t_dongle *d2, long now)
{
	if (d1->taken || d2->taken)
		return (false);
	if ((now - d1->last_release) < coder->rules->dongle_cooldown)
		return (false);
	if ((now - d2->last_release) < coder->rules->dongle_cooldown)
		return (false);
	if (d1->queue->size == 0 || d2->queue->size == 0)
		return (false);
	if (!d1->queue->nodes[0].coder || !d2->queue->nodes[0].coder)
		return (false);
	if (d1->queue->nodes[0].coder->id != coder->id)
		return (false);
	if (d2->queue->nodes[0].coder->id != coder->id)
		return (false);
	return (true);
}

long	calculate_wait_timeout(t_dongle *d1, t_dongle *d2,
								long now, long cooldown)
{
	long	t1;
	long	t2;
	long	timeout;

	if (d1->taken || d2->taken)
		return (100);
	t1 = (d1->last_release + cooldown) - now;
	t2 = (d2->last_release + cooldown) - now;
	if (t1 < t2)
		timeout = t1;
	else
		timeout = t2;
	if (timeout < 0)
		timeout = 0;
	if (timeout > 100)
		timeout = 100;
	return (timeout);
}

int	take_dongles(t_coder *coder)
{
	t_dongle		*d1;
	t_dongle		*d2;
	long			now;

	order_dongles(coder, &d1, &d2);
	insert_dongles(coder, &d1, &d2);
	while (!is_simulation_stopped(coder->rules))
	{
		pthread_mutex_lock(&d1->mutex);
		pthread_mutex_lock(&d2->mutex);
		now = get_time();
		if (can_acquire(coder, d1, d2, now))
		{
			d1->taken = true;
			d2->taken = true;
			heap_extract_min(d1->queue);
			heap_extract_min(d2->queue);
			print_dongles(coder, "has taken a dongle");
			pthread_mutex_unlock(&d2->mutex);
			pthread_mutex_unlock(&d1->mutex);
			return (0);
		}
		wait_for_signal(coder, d1, d2, now);
	}
	return (1);
}

void	put_dongles(t_coder *coder)
{
	t_dongle	*d1;
	t_dongle	*d2;
	t_coder		*next1;
	t_coder		*next2;

	order_dongles(coder, &d1, &d2);
	pthread_mutex_lock(&d1->mutex);
	pthread_mutex_lock(&d2->mutex);
	d1->taken = false;
	d1->last_release = get_time();
	d2->taken = false;
	d2->last_release = get_time();
	next1 = NULL;
	next2 = NULL;
	if (d1->queue->size > 0)
		next1 = d1->queue->nodes[0].coder;
	if (d2->queue->size > 0)
		next2 = d2->queue->nodes[0].coder;
	pthread_mutex_unlock(&d2->mutex);
	pthread_mutex_unlock(&d1->mutex);
	notify_coders(next1, next2);
}
