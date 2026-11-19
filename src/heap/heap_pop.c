/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_pop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 10:27:00 by irivas-v          #+#    #+#             */
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
** Restaura la propiedad de Min-Heap haciendo descender el nodo raiz.
** Recibe:   heap (cola de prioridad), i (indice actual) e is_edf (modo).
** Procesa:  Compara el nodo con sus hijos validos (izquierdo y derecho)
**           e intercambia con el mas prioritario hasta equilibrar el arbol.
** Devuelve: Nada (void).
*/
static void	heapify_down(t_heap *heap, int i, int is_edf)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		best = i;
		left = 2 * i + 1;
		right = 2 * i + 2;
		if (left < heap->size && has_higher_priority(&heap->nodes[left],
				&heap->nodes[best], is_edf))
			best = left;
		if (right < heap->size && has_higher_priority(&heap->nodes[right],
				&heap->nodes[best], is_edf))
			best = right;
		if (best != i)
		{
			swap_nodes(&heap->nodes[i], &heap->nodes[best]);
			i = best;
		}
		else
			break ;
	}
}

/*
** Extrae el elemento de mayor prioridad (la raiz nodes[0]) del monticulo.
** Recibe:   heap (cola de prioridad), out (salida opcional) e is_edf.
** Procesa:  Copia la raiz en *out si no es NULL, mueve el ultimo nodo
**           a la raiz, decrementa el tamano y ejecuta heapify_down.
** Devuelve: 1 si se extrajo un elemento, 0 si la cola estaba vacia.
*/
int	heap_pop(t_heap *heap, t_heap_node *out, int is_edf)
{
	if (!heap || heap->size <= 0)
		return (0);
	if (out)
		*out = heap->nodes[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->nodes[0] = heap->nodes[heap->size];
		heapify_down(heap, 0, is_edf);
	}
	return (1);
}
