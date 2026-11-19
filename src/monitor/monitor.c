/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 11:26:23 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/13 11:45:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Despierta a todos los hilos coders bloqueados en las colas de los dongles.
** Al despertar, reevaluan simulation_stop, salen de su rutina y permiten
** que pthread_join en main() se desbloquee limpiamente.
*/
static void	broadcast_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->rules.num_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].lock);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].lock);
		i++;
	}
}

/*
** Detiene la simulacion e imprime el mensaje obligatorio de burnout.
** Para evitar que otros hilos impriman despues del colapso:
** 1. Bloquea log_lock antes de alterar la bandera.
** 2. Activa simulation_stop bajo el cerrojo.
** 3. Imprime directamente el mensaje de muerte.
** 4. Desbloquea log_lock y emite broadcast a todos los dongles.
*/
static void	stop_with_burnout(t_sim *sim, int coder_id)
{
	long	timestamp;

	pthread_mutex_lock(&sim->rules.log_lock);
	if (!is_simulation_stopped(&sim->rules))
	{
		set_simulation_stop(&sim->rules);
		timestamp = get_time_ms() - sim->rules.start_time;
		printf("%ld %d %s\n", timestamp, coder_id, MSG_BURNOUT);
	}
	pthread_mutex_unlock(&sim->rules.log_lock);
	broadcast_all_dongles(sim);
}

/*
** Inspecciona el estado de un coder individual:
** - Copia compile_count y last_compile_time bajo state_lock y lo suelta.
** - Devuelve  1 si ya alcanzo compiles_required (meta cumplida).
** - Devuelve -1 si ha superado time_to_burnout (colapso por inactividad).
** - Devuelve  0 si sigue activo y dentro de plazo.
*/
static int	check_coder(t_sim *sim, int i)
{
	int		compile_count;
	long	last_compile_time;

	pthread_mutex_lock(&sim->coders[i].state_lock);
	compile_count = sim->coders[i].compile_count;
	last_compile_time = sim->coders[i].last_compile_time;
	pthread_mutex_unlock(&sim->coders[i].state_lock);
	if (sim->rules.compiles_required <= compile_count)
		return (1);
	if (get_time_ms() - last_compile_time >= sim->rules.time_to_burnout)
		return (-1);
	return (0);
}

/*
** Recorre la mesa de programadores evaluando su estado:
** - Ante el primer burnout (-1): detiene la simulacion y retorna 1.
** - Si todos los coders alcanzaron su meta: activa parada limpia y retorna 1.
** - Si la simulacion debe continuar: retorna 0.
*/
static int	check_all_coders(t_sim *sim)
{
	int	i;
	int	status;
	int	finished;

	i = 0;
	finished = 0;
	while (i < sim->rules.num_coders)
	{
		status = check_coder(sim, i);
		if (status == -1)
		{
			stop_with_burnout(sim, sim->coders[i].id);
			return (1);
		}
		if (status == 1)
			finished++;
		i++;
	}
	if (finished == sim->rules.num_coders)
	{
		set_simulation_stop(&sim->rules);
		broadcast_all_dongles(sim);
		return (1);
	}
	return (0);
}

/*
** Bucle principal del hilo supervisor (monitor):
** - Evalua a todos los programadores periodicamente.
** - Realiza microesperas de 500 us (0.5 ms) para asegurar deteccion
**   en menos de 10 ms sin provocar espera activa (0% de consumo CPU).
*/
void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_simulation_stopped(&sim->rules))
	{
		if (check_all_coders(sim))
			break ;
		usleep(500);
	}
	return (NULL);
}
