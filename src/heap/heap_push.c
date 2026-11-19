/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_push.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 10:50:01 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Intercambia los datos de dos nodos del monticulo en memoria.
** Recibe:   a (puntero al primer nodo) y b (puntero al segundo nodo).
** Procesa:  Copia temporalmente el contenido de *a y realiza el intercambio.
** Devuelve: Nada (void).
*/
static void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
** Restaura la propiedad de Min-Heap haciendo ascender el nodo en la posicion i.
** Recibe:   heap (cola de prioridad), i (indice del nodo) e is_edf (modo).
** Procesa:  Compara el nodo con su padre mediante has_higher_priority.
**           Si el nodo tiene mayor prioridad, intercambia y asciende.
** Devuelve: Nada (void).
*/
static void	heapify_up(t_heap *heap, int i, int is_edf)
{
	int	parent;

	while (i != 0)
	{
		parent = (i - 1) / 2;
		if (has_higher_priority(&heap->nodes[parent], &heap->nodes[i], is_edf))
			break ;
		swap_nodes(&heap->nodes[parent], &heap->nodes[i]);
		i = parent;
	}
}

/*
** Inserta una nueva peticion en la cola de prioridad del dongle.
** Recibe:   heap (cola de prioridad), coder_id, priority e is_edf.
** Procesa:  Verifica capacidad, inserta el nodo al final del array y ejecuta
**           heapify_up para colocarlo en su posicion correcta en el arbol.
** Devuelve: 1 si se inserto con exito, 0 si la cola esta llena.
*/
int	heap_push(t_heap *heap, int coder_id, long priority, int is_edf)
{
	int	i;

	if (heap->size >= heap->capacity)
		return (0);
	i = heap->size;
	heap->nodes[i].coder_id = coder_id;
	heap->nodes[i].priority = priority;
	heap->size += 1;
	heapify_up(heap, i, is_edf);
	return (1);
}
