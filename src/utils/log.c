/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 17:28:07 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Consulta de forma segura si la simulacion ha sido detenida.
** Recibe:   rules (puntero a la configuracion y cerrojos compartidos).
** Procesa:  Adquiere stop_lock, lee el valor de simulation_stop y desbloquea.
** Devuelve: 1 si la simulacion debe parar, 0 si sigue activa.
*/
int	is_simulation_stopped(t_rules *rules)
{
	int	stop;

	pthread_mutex_lock(&rules->stop_lock);
	stop = rules->simulation_stop;
	pthread_mutex_unlock(&rules->stop_lock);
	return (stop);
}

/*
** Activa de forma segura la bandera global de parada de la simulacion.
** Recibe:   rules (puntero a la configuracion y cerrojos compartidos).
** Procesa:  Adquiere stop_lock, asigna simulation_stop = 1 y desbloquea.
** Devuelve: Nada (void).
*/
void	set_simulation_stop(t_rules *rules)
{
	pthread_mutex_lock(&rules->stop_lock);
	rules->simulation_stop = 1;
	pthread_mutex_unlock(&rules->stop_lock);
}

/*
** Imprime un mensaje de estado serializado evitando solapamiento de lineas.
** Recibe:   coder (puntero al programador) y msg (texto de estado).
** Procesa:  Adquiere log_lock y, si la simulacion sigue activa, calcula el
**           timestamp relativo e imprime "<tiempo> <id> <mensaje>".
** Devuelve: Nada (void).
*/
void	log_action(t_coder *coder, const char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&coder->rules->log_lock);
	if (!is_simulation_stopped(coder->rules))
	{
		timestamp = get_time_ms() - coder->rules->start_time;
		printf("%ld %d %s\n", timestamp, coder->id, msg);
	}
	pthread_mutex_unlock(&coder->rules->log_lock);
}
