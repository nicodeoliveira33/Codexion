#include "codexion.h"

void	insert_dongles(t_coder *coder, t_dongle **d1, t_dongle **d2)
{
	long	key;
	int		seq1;
	int		seq2;

	pthread_mutex_lock(&(*d1)->mutex);
	pthread_mutex_lock(&(*d2)->mutex);
	key = calculate_key(coder);
	seq1 = (*d1)->sequence_counter++;
	seq2 = (*d2)->sequence_counter++;
	heap_insert((*d1)->queue, coder, key, seq1);
	heap_insert((*d2)->queue, coder, key, seq2);
	pthread_mutex_unlock(&(*d2)->mutex);
	pthread_mutex_unlock(&(*d1)->mutex);
}

void	notify_coders(t_coder *top1, t_coder *top2)
{
	if (top1)
	{
		pthread_mutex_lock(&top1->mutex);
		top1->notified = true;
		pthread_cond_signal(&top1->wake_up);
		pthread_mutex_unlock(&top1->mutex);
	}
	if (top2)
	{
		pthread_mutex_lock(&top2->mutex);
		top2->notified = true;
		pthread_cond_signal(&top2->wake_up);
		pthread_mutex_unlock(&top2->mutex);
	}
}

void	wait_for_signal(t_coder *coder, t_dongle *d1, t_dongle *d2, long now)
{
	long			timeout_ms;
	struct timespec	ts;
	int				rc;

	timeout_ms = calculate_timeout(d1, d2, now,
			coder->rules->dongle_cooldown);
	ts.tv_sec = timeout_ms / 1000;
	ts.tv_nsec = (timeout_ms % 1000) * 1000000;
	pthread_mutex_unlock(&d2->mutex);
	pthread_mutex_unlock(&d1->mutex);
	pthread_mutex_lock(&coder->mutex);
	if (!coder->notified && !is_simulation_stopped(coder->rules))
	{
		rc = pthread_cond_timedwait(&coder->wake_up,
				&coder->mutex, &ts);
		(void)rc;
	}
	coder->notified = false;
	pthread_mutex_unlock(&coder->mutex);
}
