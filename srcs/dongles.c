/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 15:38:36 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/12 16:12:55 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	find_deadline(t_rules *rules, t_coder *coder)
{
	long		left;
	long		right;
	long		deadline;

	left = coder->left_dongle->last_release;
	right = coder->right_dongle->last_release;
	if (left > right)
		deadline = left;
	else
		deadline = right;
	deadline += rules->dongle_cooldown;
	return (deadline);
}

static void	cond_selector(t_rules *rules, t_coder *coder)
{
	struct timespec	ts;
	long			deadline;
	t_dongle		*left;
	t_dongle		*right;

	left = coder->left_dongle;
	right = coder->right_dongle;
	deadline = find_deadline(rules, coder);
	convert_time(deadline, &ts);
	if (!left->taken && !right->taken
		&& get_time() < deadline
		&& !has_rival(rules, coder))
		pthread_cond_timedwait(&coder->wake_up, &rules->global_mutex, &ts);
	else
		pthread_cond_wait(&coder->wake_up, &rules->global_mutex);
}

static bool	is_available(t_rules *rules, t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = coder->left_dongle;
	right = coder->right_dongle;
	if (left->taken || right->taken
		|| get_time() - left->last_release < rules->dongle_cooldown
		|| get_time() - right->last_release < rules->dongle_cooldown
		|| has_rival(rules, coder))
		return (false);
	return (true);
}

int	take_dongles(t_rules *rules, t_coder *coder)
{
	pthread_mutex_lock(&rules->global_mutex);
	coder->request_order = ++rules->request_counter;
	if (heap_insert(rules->wait_queue, coder, rules->scheduler) != 0)
		return (pthread_mutex_unlock(&rules->global_mutex), 1);
	while (!is_available(rules, coder) && !is_simulation_stopped(rules))
		cond_selector(rules, coder);
	if (is_simulation_stopped(rules))
		return (pthread_mutex_unlock(&rules->global_mutex), 1);
	heap_remove(rules->wait_queue, coder, rules->scheduler);
	coder->left_dongle->taken = true;
	coder->right_dongle->taken = true;
	pthread_mutex_lock(&rules->print_mutex);
	print_take_and_compile(coder, "has taken a dongle");
	print_take_and_compile(coder, "has taken a dongle");
	return (0);
}

void	put_dongles(t_rules *rules, t_coder *coder)
{
	pthread_mutex_lock(&rules->global_mutex);
	coder->left_dongle->taken = false;
	coder->right_dongle->taken = false;
	coder->left_dongle->last_release = get_time();
	coder->right_dongle->last_release = coder->left_dongle->last_release;
	wake_rivals(rules, coder);
	pthread_mutex_unlock(&rules->global_mutex);
}
