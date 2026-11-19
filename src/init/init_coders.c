/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 12:09:15 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Asigna los dongles izquierdo y derecho ordenados por jerarquia de Dijkstra.
** Recibe:   coder (puntero al programador), sim y su indice i.
** Procesa:  Identifica los dongles contiguos y asigna first_dongle al de menor
**           id y second_dongle al de mayor id, rompiendo la espera circular.
**           En el caso especial num_coders == 1, fija second_dongle a NULL.
** Devuelve: Nada (void).
*/
static void	assign_dongles(t_coder *coder, t_sim *sim, int i)
{
	t_dongle	*left;
	t_dongle	*right;

	left = &sim->dongles[i];
	right = &sim->dongles[(i + 1) % sim->rules.num_coders];
	if (sim->rules.num_coders == 1)
	{
		coder->first_dongle = left;
		coder->second_dongle = NULL;
	}
	else if (left->id < right->id)
	{
		coder->first_dongle = left;
		coder->second_dongle = right;
	}
	else
	{
		coder->first_dongle = right;
		coder->second_dongle = left;
	}
}

/*
** Reserva e inicializa el array completo de programadores.
** Recibe:   sim (puntero a la estructura principal).
** Procesa:  Reserva memoria dinamica para num_coders estructuras t_coder,
**           fija id (i + 1), inicializa state_lock y asigna dongles.
** Devuelve: 1 si todos los coders se inicializaron, 0 si falla el malloc.
*/
int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->rules.num_coders);
	if (!sim->coders)
		return (0);
	i = 0;
	while (i < sim->rules.num_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_time = 0;
		sim->coders[i].rules = &sim->rules;
		pthread_mutex_init(&sim->coders[i].state_lock, NULL);
		assign_dongles(&sim->coders[i], sim, i);
		i++;
	}
	return (1);
}
