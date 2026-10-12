/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/08 19:38:58 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/12 17:31:48 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	coder_shares_dongles(t_coder *a, t_coder *b)
{
	return (a->left_dongle == b->left_dongle
		|| a->left_dongle == b->right_dongle
		|| a->right_dongle == b->left_dongle
		|| a->right_dongle == b->right_dongle);
}

bool	has_rival(t_rules *rules, t_coder *coder)
{
	int		i;
	t_coder	*other;

	i = 0;
	while (i < rules->wait_queue->size)
	{
		other = rules->wait_queue->coders[i];
		if (other != coder
			&& coder_shares_dongles(other, coder)
			&& has_priority(other, coder, rules->scheduler)
			&& !other->left_dongle->taken
			&& !other->right_dongle->taken)
			return (true);
		i++;
	}
	return (false);
}

void	wake_rivals(t_rules *rules, t_coder *coder)
{
	int		i;
	t_coder	*other;

	i = 0;
	while (i < rules->wait_queue->size)
	{
		other = rules->wait_queue->coders[i];
		if (other != coder
			&& coder_shares_dongles(other, coder))
			pthread_cond_signal(&other->wake_up);
		i++;
	}
}
