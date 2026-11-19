/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   release_dongles.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 17:51:23 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Protocolo de liberacion de un dongle y activacion del cooldown.
** Recibe:   d (dongle a liberar) y cooldown_ms (tiempo de enfriamiento en ms).
** Procesa:  Marca in_use = 0, fija cooldown_until, despierta a todos los
**           hilos en cola con cond_broadcast y desbloquea el cerrojo.
** Devuelve: Nada (void).
*/
void	release_dongle(t_dongle *d, long cooldown_ms)
{
	if (!d)
		return ;
	pthread_mutex_lock(&d->lock);
	d->in_use = 0;
	d->cooldown_until = get_time_ms() + cooldown_ms;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->lock);
}
