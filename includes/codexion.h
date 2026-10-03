#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>
# include <stdint.h>
# include <stdbool.h>

/* Forward Declarations para evitar erros de dependência circular */
typedef struct s_rules		t_rules;
typedef struct s_coder		t_coder;
typedef struct s_heap		t_heap;
typedef struct s_heap_node	t_heap_node;

typedef enum e_scheduler {
	POLICY_FIFO,
	POLICY_EDF
}	t_scheduler;

struct s_rules {
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
	pthread_mutex_t	state_mutex;
	pthread_mutex_t	*dongles;
	pthread_cond_t	queue_cond;
	long			*dongle_last_use;
	pthread_mutex_t	print_mutex;
	pthread_t		monitor;
	t_heap			*priority_queue;
};

struct s_coder {
	int				id;
	pthread_t		thread;
	int				compiles_done;
	long			last_compile_start;
	t_rules			*rules;
	int				left_dongle_idx;
	int				right_dongle_idx;
	pthread_mutex_t	coder_mutex;
};

struct s_heap_node {
	t_coder	*coder;
	long	key;
	int		id_tie;
};

struct s_heap {
	t_heap_node	*nodes;
	int			capacity;
	int			size;
};

int		parser(int ac, char **av, t_rules *rules);

int		init_simulation(t_rules *rules, t_coder **coders);

bool	is_simulation_stopped(t_rules *rules);
void	set_simulation_stop(t_rules *rules);
int		start_simulation(t_rules *rules, t_coder *coders);

void	*coder_routine(void *arg);
void	*monitor_routine(void *arg);

void	take_dongles(t_coder *coder);
void	put_dongles(t_coder *coder);
void	take_dongle(t_coder *coder);

long	get_time(void);
void	ft_usleep(long time_in_ms, t_rules *rules);

void	print_status(t_coder *coder, char *status);
void	print_burnout(t_coder *coder, char *str);
void	print_invalid_args(void);

t_heap	*init_heap(int capacity);
void	heap_insert(t_heap *heap, t_coder *coder, long key);
t_coder	*heap_extract_min(t_heap *heap);

bool	should_swap(t_heap_node parent, t_heap_node child);
void	swap_nodes(t_heap_node *a, t_heap_node *b);
long	calculate_key(t_coder *coder);
void	heapify_down(t_heap *heap, int i);

void	destroy_all_rules_mutexes(t_rules *rules);
void	free_all(t_rules *rules, t_coder *coders);
void	cleanup(t_rules *rules, t_coder **coders);

#endif