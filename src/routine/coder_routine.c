/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 22:27:10 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Rutina de espera y colapso para el caso borde de un unico programador.
** Recibe:   c (puntero al unico coder de la simulacion).
** Procesa:  Toma su unico dongle y duerme durante time_to_burnout hasta morir.
**           Libera el dongle antes de finalizar para evitar fugas.
** Devuelve: NULL.
*/
static void	*single_coder(t_coder *c)
{
	take_dongle(c->first_dongle, c);
	precise_usleep(c->rules->time_to_burnout, c->rules);
	release_dongle(c->first_dongle, 0);
	return (NULL);
}

/*
** Comprueba de forma segura si el programador alcanzo la meta de compilaciones.
** Recibe:   c (puntero al programador).
** Procesa:  Lee compile_count bajo state_lock y compara con compiles_required.
** Devuelve: 1 si alcanzo la meta fijada, 0 en caso contrario.
*/
static int	has_reached_goal(t_coder *c)
{
	int	reached;

	reached = 0;
	pthread_mutex_lock(&c->state_lock);
	if (c->rules->compiles_required > 0
		&& c->compile_count >= c->rules->compiles_required)
		reached = 1;
	pthread_mutex_unlock(&c->state_lock);
	return (reached);
}

/*
** Ejecuta una ronda completa del ciclo de trabajo de un programador.
** Recibe:   c (puntero al programador).
** Procesa:  Adquiere ambos dongles, compila, libera con cooldown y, si no ha
**           alcanzado su meta, depura y refactoriza comprobando paradas.
** Devuelve: 1 si completo el ciclo y debe seguir, 0 si debe detenerse.
*/
static int	coder_cycle(t_coder *c)
{
	if (!take_dongle(c->first_dongle, c))
		return (0);
	if (!take_dongle(c->second_dongle, c))
	{
		release_dongle(c->first_dongle, c->rules->dongle_cooldown);
		return (0);
	}
	compile_action(c);
	release_dongle(c->first_dongle, c->rules->dongle_cooldown);
	release_dongle(c->second_dongle, c->rules->dongle_cooldown);
	if (is_simulation_stopped(c->rules) || has_reached_goal(c))
		return (0);
	do_action(c, c->rules->time_to_debug, MSG_DEBUG);
	if (is_simulation_stopped(c->rules))
		return (0);
	do_action(c, c->rules->time_to_refactor, MSG_REFACTOR);
	return (1);
}

/*
** Punto de entrada del hilo de ejecucion de cada programador.
** Recibe:   arg (puntero generico a t_coder).
** Procesa:  Desincroniza a los pares (usleep 1000), e itera coder_cycle
**           hasta la parada o hasta completar compiles_required.
** Devuelve: NULL.
*/
void	*coder_routine(void *arg)
{
	t_coder	*c;

	c = (t_coder *)arg;
	if (c->rules->num_coders == 1)
		return (single_coder(c));
	if (c->id % 2 == 0)
		usleep(1000);
	while (!is_simulation_stopped(c->rules))
	{
		if (!coder_cycle(c))
			break ;
	}
	return (NULL);
}
