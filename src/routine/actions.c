/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 22:13:16 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Ejecuta una accion generica de trabajo (depurar o refactorizar).
** Recibe:   c (puntero al programador), duration_ms y msg (texto de estado).
** Procesa:  Emite el log obligatorio y duerme con precise_usleep.
** Devuelve: Nada (void).
*/
void	do_action(t_coder *c, long duration_ms, const char *msg)
{
	log_action(c, msg);
	precise_usleep(duration_ms, c->rules);
}

/*
** Ejecuta la accion de compilar reteniendo ambos dongles.
** Recibe:   c (puntero al programador).
** Procesa:  Actualiza last_compile_time al inicio bajo state_lock, emite
**           log, duerme time_to_compile e incrementa compile_count al acabar.
** Devuelve: Nada (void).
*/
void	compile_action(t_coder *c)
{
	pthread_mutex_lock(&c->state_lock);
	c->last_compile_time = get_time_ms();
	pthread_mutex_unlock(&c->state_lock);
	log_action(c, MSG_COMPILE);
	precise_usleep(c->rules->time_to_compile, c->rules);
	pthread_mutex_lock(&c->state_lock);
	c->compile_count++;
	pthread_mutex_unlock(&c->state_lock);
}
