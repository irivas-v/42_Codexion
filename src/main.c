/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 10:43:38 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:30:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

/*
** Inicializa el reloj base de la simulacion y sincroniza los plazos iniciales
** de todos los programadores antes de crear los hilos concurrentes.
** Posteriormente lanza los hilos de los coders y el hilo supervisor (monitor).
*/
static void	start_simulation(t_sim *sim)
{
	int	i;

	sim->rules.start_time = get_time_ms();
	i = 0;
	while (i < sim->rules.num_coders)
	{
		sim->coders[i].last_compile_time = sim->rules.start_time;
		i++;
	}
	i = 0;
	while (i < sim->rules.num_coders)
	{
		pthread_create(&sim->coders[i].thread, NULL,
			coder_routine, &sim->coders[i]);
		i++;
	}
	pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim);
}

/*
** Bloquea el hilo principal (main) hasta que todos los hilos secundarios
** hayan concluido formalmente su ejecucion mediante return (NULL).
** Espera primero a los coders y finalmente al hilo monitor.
*/
static void	wait_simulation(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->rules.num_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(sim->monitor_thread, NULL);
}

/*
** Punto de entrada del programa:
** 1. Valida los 8 argumentos obligatorios mediante parse_args.
** 2. Inicializa las estructuras dinamicas, mutexes y condiciones (init_sim).
** 3. Arranca todos los hilos de ejecucion (start_simulation).
** 4. Espera la finalizacion completa de todos los hilos (wait_simulation).
** 5. Destruye cerrojos y libera toda la memoria reservada en el Heap.
*/
int	main(int argc, char **argv)
{
	t_sim	sim;

	if (argc != 9 || parse_args(argc, argv, &sim.rules) != 0)
	{
		printf("Error: Invalid arguments\n");
		return (1);
	}
	if (!init_simulation(&sim))
		return (1);
	start_simulation(&sim);
	wait_simulation(&sim);
	cleanup_simulation(&sim);
	return (0);
}
