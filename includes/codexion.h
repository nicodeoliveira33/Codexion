#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>


typedef enum e_sched
{
    POLICY_FIFO,
	POLICY_EDF
}   t_sched;

typedef struct s_request
{
    int     coder_id;
    long    arrival_time;
    long    deadline;
}   t_request;

typedef struct s_heap
{
    t_request   *data;
    int         size;
    int         capacity;
}   t_heap;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long			last_released;
	int				taken;
	int				current_holder_id;
	t_heap			queue;
}				t_dongle;

typedef struct s_rules
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;
	t_sched			scheduler;
	long			start_time;
	int				stop_simulation;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	write_mutex;
	t_dongle		*dongles;
	pthread_t		monitor;
}				t_rules;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				compiled;
	long			last_compile_start;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_rules			*rules;
	pthread_mutex_t	coder_mutex;
}				t_coder;



int 	parser(int ac, char **av, t_rules *config);

int		init_simulation(t_rules *rules, t_coder **coders);
void	start_simulation(t_rules *rules, t_coder *coders);
void	set_simulation_stop(t_rules *rules);
int 	is_simulation_stopped(t_rules *rules);


int		heap_init(t_heap *heap, int capacity);
void	heap_destroy(t_heap *heap);
void	heap_insert(t_heap *heap, t_request req, t_sched policy);
t_request	heap_extract_min(t_heap *heap, t_sched policy);

void	*coder_routine(void *arg);
void	debug(t_coder *coder);
void	refactor(t_coder *coder);

int		take_dongles(t_coder *coder);
void 	put_dongles(t_coder *coder);
void	compile(t_coder *coder);

void	*monitor_routine(void *arg);

long	get_time(void);
void	ft_usleep(long time_in_ms, t_rules *rules);

void	print_status(t_coder *coder, char *status);
void	print_burnout(t_coder *coder, char *str);

void	cleanup(t_rules *rules, t_coder *coders);

#endif