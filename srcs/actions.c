#include "codexion.h"

static int	try_lock_both(t_coder *coder, t_dongle *f, t_dongle *s)
{
	long	now;

	pthread_mutex_lock(&f->mutex);
	pthread_mutex_lock(&s->mutex);
	now = get_time();
	if (!f->taken && !s->taken
		&& now >= f->last_released + coder->rules->dongle_cooldown
		&& now >= s->last_released + coder->rules->dongle_cooldown)
	{
		f->taken = 1;
		s->taken = 1;
		pthread_mutex_unlock(&s->mutex);
		pthread_mutex_unlock(&f->mutex);
		print_status(coder, "has taken a dongle");
		print_status(coder, "has taken a dongle");
		return (1);
	}
	pthread_mutex_unlock(&s->mutex);
	pthread_mutex_unlock(&f->mutex);
	return (0);
}

static int	handle_single_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->taken = 1;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	print_status(coder, "has taken a dongle");
	while (!is_simulation_stopped(coder->rules))
		ft_usleep(1, coder->rules);
	return (1);
}

int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->rules->number_of_coders == 1)
		return (handle_single_coder(coder));
	first = coder->left_dongle;
	second = coder->right_dongle;
	if (first->id > second->id)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	while (!is_simulation_stopped(coder->rules))
	{
		if (try_lock_both(coder, first, second))
			return (0);
		ft_usleep(1, coder->rules);
	}
	return (1);
}

void	put_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->taken = 0;
	coder->left_dongle->last_released = get_time();
	pthread_cond_broadcast(&coder->left_dongle->cond);
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	coder->right_dongle->taken = 0;
	coder->right_dongle->last_released = get_time();
	pthread_cond_broadcast(&coder->right_dongle->cond);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}