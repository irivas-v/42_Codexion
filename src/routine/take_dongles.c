/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 13:30:21 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Duerme al hilo de forma segura hasta expirar el cooldown del dongle.
** Recibe:   d (puntero al dongle a enfriar).
** Procesa:  Calcula la marca de tiempo absoluta en timespec y duerme
**           liberando d->lock mediante pthread_cond_timedwait.
** Devuelve: Nada (void).
*/
static void	wait_cooldown(t_dongle *d)
{
	struct timeval	tv;
	struct timespec	ts;
	long			ms;

	ms = d->cooldown_until - get_time_ms();
	if (ms <= 0)
		return ;
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + (tv.tv_usec / 1000 + ms) / 1000;
	ts.tv_nsec = ((tv.tv_usec / 1000 + ms) % 1000) * 1000000;
	pthread_cond_timedwait(&d->cond, &d->lock, &ts);
}

/*
** Bloquea al programador hasta que sea su turno y el dongle este disponible.
** Recibe:   d (dongle solicitado) y c (programador solicitante).
** Procesa:  Si el dongle esta libre y c es la raiz del heap, espera el fin
**           del cooldown; si esta ocupado o no es primero, usa cond_wait.
** Devuelve: Nada (void).
*/
static void	wait_dongle_turn(t_dongle *d, t_coder *c)
{
	while (!is_simulation_stopped(c->rules))
	{
		if (!d->in_use && d->queue.size > 0
			&& d->queue.nodes[0].coder_id == c->id)
		{
			if (get_time_ms() >= d->cooldown_until)
				break ;
			wait_cooldown(d);
		}
		else
			pthread_cond_wait(&d->cond, &d->lock);
	}
}

/*
** Calcula la prioridad del coder segun el planificador configurado.
** Recibe:   c (puntero al programador).
** Procesa:  En EDF calcula deadline (last_compile_time + burnout) bajo
**           state_lock; en FIFO retorna el instante get_time_ms().
** Devuelve: Valor numerico de prioridad (a menor valor, mayor prioridad).
*/
static long	get_coder_priority(t_coder *c)
{
	long	priority;

	if (c->rules->is_edf)
	{
		pthread_mutex_lock(&c->state_lock);
		priority = c->last_compile_time + c->rules->time_to_burnout;
		pthread_mutex_unlock(&c->state_lock);
	}
	else
		priority = get_time_ms();
	return (priority);
}

/*
** Protocolo completo de adquisicion concurrente de un dongle.
** Recibe:   d (dongle solicitado) y c (programador solicitante).
** Procesa:  Encola la peticion en el Min-Heap bajo d->lock, espera su turno,
**           extrae la raiz con heap_pop, marca in_use = 1 y emite el log.
** Devuelve: 1 si adquirio el dongle, 0 si la simulacion se detuvo en espera.
*/
int	take_dongle(t_dongle *d, t_coder *c)
{
	pthread_mutex_lock(&d->lock);
	heap_push(&d->queue, c->id, get_coder_priority(c), c->rules->is_edf);
	wait_dongle_turn(d, c);
	if (is_simulation_stopped(c->rules))
	{
		pthread_mutex_unlock(&d->lock);
		return (0);
	}
	heap_pop(&d->queue, NULL, c->rules->is_edf);
	d->in_use = 1;
	pthread_mutex_unlock(&d->lock);
	log_action(c, MSG_TAKE_DONGLE);
	return (1);
}
