/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 14:32:09 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Inicializa la memoria, cerrojos y cola de prioridad de un unico dongle.
** Recibe:   sim (puntero a la simulacion) e i (indice del dongle).
** Procesa:  Asigna id (i + 1), in_use = 0, reserva memoria para los nodos del
**           Min-Heap e inicializa el mutex lock y la condicion cond.
** Devuelve: 1 si la inicializacion fue exitosa, 0 si falla el malloc.
*/
static int	init_single_dongle(t_sim *sim, int i)
{
	t_dongle	*d;

	d = &sim->dongles[i];
	d->id = i + 1;
	d->in_use = 0;
	d->cooldown_until = 0;
	d->queue.capacity = sim->rules.num_coders;
	d->queue.size = 0;
	d->queue.nodes = malloc(sizeof(t_heap_node) * sim->rules.num_coders);
	if (!d->queue.nodes)
		return (0);
	pthread_mutex_init(&d->lock, NULL);
	pthread_cond_init(&d->cond, NULL);
	return (1);
}

/*
** Reserva e inicializa el array completo de dongles de la simulacion.
** Recibe:   sim (puntero a la estructura principal).
** Procesa:  Reserva memoria dinamica para num_coders dongles e inicializa
**           cada uno; ante cualquier fallo realiza rollback con cleanup.
** Devuelve: 1 si todos los dongles se inicializaron, 0 en caso de fallo.
*/
int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->rules.num_coders);
	if (!sim->dongles)
		return (0);
	i = 0;
	while (i < sim->rules.num_coders)
	{
		if (!init_single_dongle(sim, i))
		{
			cleanup_dongles_upto(sim->dongles, i);
			sim->dongles = NULL;
			return (0);
		}
		i++;
	}
	return (1);
}
