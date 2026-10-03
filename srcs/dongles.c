#include "codexion.h"

static int	check_cooldown(t_rules *rules, int idx)
{
	long	now;

	now = get_time();
	if (rules->dongle_last_use[idx] == -1)
		return (1); // Ocupado
	if ((now - rules->dongle_last_use[idx]) < rules->dongle_cooldown)
		return (2); // Em cooldown
	return (0); // Disponível
}

static void	wait_in_queue(t_coder *c, int d1, int d2)
{
	pthread_mutex_lock(&c->rules->state_mutex);
	while (!c->rules->sim_stop)
	{
		if (c->rules->priority_queue->size > 0
			&& c->rules->priority_queue->nodes[0].coder->id == c->id)
		{
			if (check_cooldown(c->rules, d1) == 0 && check_cooldown(c->rules, d2) == 0)
			{
				c->rules->dongle_last_use[d1] = -1;
				c->rules->dongle_last_use[d2] = -1;
				heap_extract_min(c->rules->priority_queue);
				break ;
			}
			if (check_cooldown(c->rules, d1) != 1 && check_cooldown(c->rules, d2) != 1)
			{
				pthread_mutex_unlock(&c->rules->state_mutex);
				usleep(500);
				pthread_mutex_lock(&c->rules->state_mutex);
				continue ;
			}
		}
		pthread_cond_wait(&c->rules->queue_cond, &c->rules->state_mutex);
	}
	pthread_mutex_unlock(&c->rules->state_mutex);
}

void	take_dongles(t_coder *coder)
{
	long	key;
	int		d1;
	int		d2;

	key = calculate_key(coder);
	d1 = coder->left_dongle_idx;
	d2 = coder->right_dongle_idx;
	if (d1 > d2)
	{
		d1 = coder->right_dongle_idx;
		d2 = coder->left_dongle_idx;
	}
	pthread_mutex_lock(&coder->rules->state_mutex);
	heap_insert(coder->rules->priority_queue, coder, key);
	pthread_cond_broadcast(&coder->rules->queue_cond);
	pthread_mutex_unlock(&coder->rules->state_mutex);
	
	wait_in_queue(coder, d1, d2);
	
	pthread_mutex_lock(&coder->rules->dongles[d1]);
	print_status(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->rules->dongles[d2]);
	print_status(coder, "has taken a dongle");
}

void	put_dongles(t_coder *coder)
{
	long	now;

	now = get_time();
	pthread_mutex_lock(&coder->rules->state_mutex);
	coder->rules->dongle_last_use[coder->left_dongle_idx] = now;
	coder->rules->dongle_last_use[coder->right_dongle_idx] = now;
	pthread_cond_broadcast(&coder->rules->queue_cond);
	pthread_mutex_unlock(&coder->rules->state_mutex);
	
	pthread_mutex_unlock(&coder->rules->dongles[coder->left_dongle_idx]);
	pthread_mutex_unlock(&coder->rules->dongles[coder->right_dongle_idx]);
}

void	take_dongle(t_coder *coder)
{
	pthread_mutex_lock(&coder->rules->dongles[coder->left_dongle_idx]);
	print_status(coder, "has taken a dongle");
	ft_usleep(coder->rules->time_to_burnout, coder->rules);
	pthread_mutex_unlock(&coder->rules->dongles[coder->left_dongle_idx]);
}