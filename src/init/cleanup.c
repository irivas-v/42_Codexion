/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 13:36:00 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Destruye y libera los dongles desde el indice 0 hasta count - 1.
** Recibe:   dongles (array de dongles) y count (elementos a liberar).
** Procesa:  Libera la cola heap.nodes, destruye lock y cond por cada dongle,
**           y finalmente libera el bloque principal con free(dongles).
** Devuelve: Nada (void).
*/
void	cleanup_dongles_upto(t_dongle *dongles, int count)
{
	int	j;

	if (!dongles)
		return ;
	j = 0;
	while (j < count)
	{
		if (dongles[j].queue.nodes)
		{
			free(dongles[j].queue.nodes);
			dongles[j].queue.nodes = NULL;
		}
		pthread_mutex_destroy(&dongles[j].lock);
		pthread_cond_destroy(&dongles[j].cond);
		j++;
	}
	free(dongles);
}

/*
** Destruye los cerrojos y libera los coders desde el indice 0 hasta count - 1.
** Recibe:   coders (array de programadores) y count (cantidad de elementos).
** Procesa:  Destruye state_lock de cada coder y libera el array con free.
** Devuelve: Nada (void).
*/
static void	cleanup_coders_upto(t_coder *coders, int count)
{
	int	j;

	if (!coders)
		return ;
	j = 0;
	while (j < count)
	{
		pthread_mutex_destroy(&coders[j].state_lock);
		j++;
	}
	free(coders);
}

/*
** Destructor global y centralizado de la simulacion.
** Recibe:   sim (puntero a la estructura raiz t_sim).
** Procesa:  Invoca cleanup_dongles_upto, cleanup_coders_upto y destruye
**           los cerrojos globales stop_lock y log_lock. Anula punteros a NULL.
** Devuelve: Nada (void).
*/
void	cleanup_simulation(t_sim *sim)
{
	if (!sim)
		return ;
	cleanup_dongles_upto(sim->dongles, sim->rules.num_coders);
	sim->dongles = NULL;
	cleanup_coders_upto(sim->coders, sim->rules.num_coders);
	sim->coders = NULL;
	pthread_mutex_destroy(&sim->rules.stop_lock);
	pthread_mutex_destroy(&sim->rules.log_lock);
}
