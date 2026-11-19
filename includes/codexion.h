/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 12:28:00 by irivas-v          #+#    #+#             */
/*   Updated: 2026/10/22 19:35:24 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

/*
** Constantes de estado para el logger
*/
# define MSG_TAKE_DONGLE	"has taken a dongle"
# define MSG_COMPILE		"is compiling"
# define MSG_DEBUG			"is debugging"
# define MSG_REFACTOR		"is refactoring"
# define MSG_BURNOUT		"burned out"

/*
** Nodo de la cola de prioridad (Min-Heap)
*/
typedef struct s_heap_node
{
	int				coder_id;
	long			priority;
}	t_heap_node;

/*
** Cola de prioridad (Min-Heap) para cada dongle
*/
typedef struct s_heap
{
	t_heap_node		*nodes;
	int				size;
	int				capacity;
}	t_heap;

/*
** Estructura de cada dongle
*/
typedef struct s_dongle
{
	int				id;
	int				in_use;
	long			cooldown_until;
	t_heap			queue;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
}	t_dongle;

/*
** Reglas globales y control de simulación (sin variables globales)
*/
typedef struct s_rules
{
	int				num_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				compiles_required;
	long			dongle_cooldown;
	int				is_edf;
	long			start_time;
	int				simulation_stop;
	pthread_mutex_t	stop_lock;
	pthread_mutex_t	log_lock;
}	t_rules;

/*
** Estructura individual de cada programador (coder)
*/
typedef struct s_coder
{
	int				id;
	int				compile_count;
	long			last_compile_time;
	pthread_mutex_t	state_lock;
	t_dongle		*first_dongle;
	t_dongle		*second_dongle;
	t_rules			*rules;
	pthread_t		thread;
}	t_coder;

/*
** Estructura principal de la simulación
*/
typedef struct s_sim
{
	t_rules			rules;
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_t		monitor_thread;
}	t_sim;

/*
** Parsing
*/
int		parse_args(int argc, char **argv, t_rules *rules);
int		ft_atoi_positive(const char *str);

/*
** Inicialización y limpieza
*/
int		init_simulation(t_sim *sim);
int		init_dongles(t_sim *sim);
int		init_coders(t_sim *sim);
void	cleanup_simulation(t_sim *sim);
void	cleanup_dongles_upto(t_dongle *dongles, int count);

/*
** Heap / Prioridad
*/
int		has_higher_priority(t_heap_node *a, t_heap_node *b, int is_edf);
int		heap_push(t_heap *heap, int coder_id, long priority, int is_edf);
int		heap_pop(t_heap *heap, t_heap_node *out, int is_edf);

/*
** Rutina de los coders
*/
void	*coder_routine(void *arg);
int		take_dongle(t_dongle *d, t_coder *c);
void	release_dongle(t_dongle *d, long cooldown_ms);
void	compile_action(t_coder *c);
void	do_action(t_coder *c, long duration_ms, const char *msg);

/*
** Monitor
*/
void	*monitor_routine(void *arg);
int		is_simulation_stopped(t_rules *rules);
void	set_simulation_stop(t_rules *rules);

/*
** Utilidades (tiempo y logs)
*/
long	get_time_ms(void);
void	precise_usleep(long duration_ms, t_rules *rules);
void	log_action(t_coder *c, const char *msg);

#endif
