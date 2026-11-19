/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_priority.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:34:52 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Determina si el nodo 'a' tiene mayor prioridad que el nodo 'b'.
** Recibe:   a (nodo candidato), b (nodo rival), is_edf (1 si EDF, 0 si FIFO).
** Procesa:  Compara la prioridad numerica (menor valor = mayor urgencia).
**           En caso de empate:
**           - En EDF (recode): desempata a favor del coder_id mas alto.
**           - En FIFO: desempata a favor del coder_id mas bajo.
** Devuelve: 1 si 'a' tiene mayor prioridad que 'b', 0 en caso contrario.
*/
int	has_higher_priority(t_heap_node *a, t_heap_node *b, int is_edf)
{
	if (a->priority < b->priority)
		return (1);
	else if (a->priority > b->priority)
		return (0);
	else
	{
		if (is_edf)
			return (a->coder_id > b->coder_id);
		else
			return (a->coder_id < b->coder_id);
	}
}
