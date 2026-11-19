/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 11:59:27 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Obtiene la estampa temporal actual del sistema en milisegundos.
** Recibe:   Nada (void).
** Procesa:  Consulta el reloj POSIX gettimeofday y convierte tv_sec a ms
**           multiplicando por 1000 y tv_usec dividiendo entre 1000.
** Devuelve: Marca de tiempo absoluta en milisegundos, o -1 si hay error.
*/
long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/*
** Realiza una espera precisa fraccionada en intervalos de 500 microsegundos.
** Recibe:   duration_ms (duracion en ms) y rules (configuracion y cerrojos).
** Procesa:  Duerme en tramos de 500 us comprobando periodicamente si
**           simulation_stop se ha activado bajo stop_lock para salir.
** Devuelve: Nada (void).
*/
void	precise_usleep(long duration_ms, t_rules *rules)
{
	long	start_time;

	start_time = get_time_ms();
	while ((get_time_ms() - start_time) < duration_ms)
	{
		pthread_mutex_lock(&rules->stop_lock);
		if (rules->simulation_stop)
		{
			pthread_mutex_unlock(&rules->stop_lock);
			break ;
		}
		pthread_mutex_unlock(&rules->stop_lock);
		usleep(500);
	}
}
