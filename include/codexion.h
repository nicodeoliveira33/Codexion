/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndias-de <ndias-de@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/06 13:04:32 by ndias-de          #+#    #+#             */
/*   Updated: 2026/10/08 20:14:29 by ndias-de         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdbool.h>
# include <sys/time.h>

typedef struct s_rules		t_rules;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;
typedef struct s_heap		t_heap;

typedef enum e_scheduler
{
	POLICY_FIFO,
	POLICY_EDF
}	t_scheduler;

struct s_heap
{
	t_coder	**coders;
	int		capacity;
	int		size;
};

struct s_dongle
{
	int		id;
	bool	taken;
	long	last_release;
};

struct s_coder
{
	int				id;
	int				compiles_done;
	long			last_compile_start;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_rules			*rules;
	long			request_order;
	pthread_mutex_t	mutex;
	pthread_cond_t	wake_up;
	pthread_t		thread;
};

struct s_rules
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;
	t_scheduler		scheduler;
	long			start_time;
	bool			sim_stop;
	pthread_mutex_t	stop_sim_mutex;
	pthread_mutex_t	global_mutex;
	pthread_mutex_t	print_mutex;
	t_heap			*wait_queue;
	long			request_counter;
	t_dongle		*dongles;
	pthread_t		monitor;
};

int		parser(int ac, char **av, t_rules *rules);

int		init_sim(t_rules *rules, t_coder **coders);

int		start_sim(t_rules *rules, t_coder *coders);

bool	is_simulation_stopped(t_rules *rules);
void	set_simulation_stop(t_rules *rules, t_coder *coders);

void	*coder_routine(void *arg);
void	*monitor_routine(void *arg);

int		heap_insert(t_heap *heap, t_coder *coder, t_scheduler scheduler);
void	heap_remove(t_heap *heap, t_coder *coder, t_scheduler scheduler);
void	heap_destroy(t_heap *heap);
bool	has_priority(t_coder *c1, t_coder *c2, t_scheduler schedule);

int		take_dongles(t_rules *rules, t_coder *coder);
void	put_dongles(t_rules *rules, t_coder *coder);

bool	has_rival(t_rules *rules, t_coder *coder);
void	wake_rivals(t_rules *rules, t_coder *coder);

void	cleanup(t_rules *rules, t_coder **coders);
void	destroy_mutex_coders(t_coder *coders, int count);
void	destroy_mutex_rules(t_rules *rules);
void	free_all(t_rules *rules, t_coder *coders);

long	get_time(void);
void	ft_usleep(long time_in_ms, t_rules *rules);
void	convert_time(long timestamp, struct timespec *ts);

void	print_invalid_args(void);
void	print_status(t_coder *coder, char *status);
void	print_burnout(t_coder *coder, char *str);

#endif
