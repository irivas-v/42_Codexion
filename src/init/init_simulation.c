/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 15:37:00 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Constructor central de la simulacion en memoria dinamica.
** Recibe:   sim (puntero a la estructura raiz t_sim).
** Procesa:  Inicializa stop_lock, log_lock y llama secuencialmente a
**           init_dongles e init_coders; ante cualquier fallo ejecuta cleanup.
** Devuelve: 1 si todo el ecosistema se inicializo con exito, 0 si hay error.
*/
int	init_simulation(t_sim *sim)
{
	if (!sim)
		return (0);
	sim->dongles = NULL;
	sim->coders = NULL;
	sim->rules.simulation_stop = 0;
	if (pthread_mutex_init(&sim->rules.stop_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->rules.log_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->rules.stop_lock);
		return (0);
	}
	if (!init_dongles(sim))
	{
		cleanup_simulation(sim);
		return (0);
	}
	if (!init_coders(sim))
	{
		cleanup_simulation(sim);
		return (0);
	}
	return (1);
}
