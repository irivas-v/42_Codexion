/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irivas-v <irivas-v@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 14:12:15 by irivas-v          #+#    #+#             */
/*   Updated: 2026/11/16 11:35:00 by irivas-v         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/codexion.h"

/*
** Convierte una cadena numerica en un entero positivo protegido de overflow.
** Recibe:   str (cadena a convertir).
** Procesa:  Verifica que solo contenga digitos y acumula el valor en un long
**           de 64 bits para detectar desbordamientos mayores a INT_MAX.
** Devuelve: El numero entero positivo, o -1 si la entrada es invalida.
*/
int	ft_atoi_positive(const char *str)
{
	int		i;
	long	result;

	result = 0;
	i = 0;
	if (!str || str[0] == '\0')
		return (-1);
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = (result * 10) + (str[i] - '0');
		if (result > 2147483647)
			return (-1);
		i++;
	}
	if (str[i] == '\0')
		return (result);
	return (-1);
}

/*
** Valida y almacena los 8 parametros obligatorios de la simulacion.
** Recibe:   argc (numero de argumentos), argv (array de strings) y rules.
** Procesa:  Comprueba que argc == 9, valida que todos los tiempos y limites
**           sean estrictamente positivos y configura el scheduler (fifo/edf).
** Devuelve: 0 si todos los parametros son validos, -1 si hay algun error.
*/
int	parse_args(int argc, char **argv, t_rules *rules)
{
	if (argc != 9)
		return (-1);
	rules->num_coders = ft_atoi_positive(argv[1]);
	rules->time_to_burnout = ft_atoi_positive(argv[2]);
	rules->time_to_compile = ft_atoi_positive(argv[3]);
	rules->time_to_debug = ft_atoi_positive(argv[4]);
	rules->time_to_refactor = ft_atoi_positive(argv[5]);
	rules->compiles_required = ft_atoi_positive(argv[6]);
	rules->dongle_cooldown = ft_atoi_positive(argv[7]);
	if (rules->num_coders < 1 || rules->time_to_burnout <= 0
		|| rules->time_to_compile < 0 || rules->time_to_debug < 0
		|| rules->time_to_refactor < 0 || rules->compiles_required < 1
		|| rules->dongle_cooldown < 0)
		return (-1);
	if (strcmp(argv[8], "fifo") == 0)
		rules->is_edf = 0;
	else if (strcmp(argv[8], "edf") == 0)
		rules->is_edf = 1;
	else
		return (-1);
	rules->start_time = 0;
	rules->simulation_stop = 0;
	return (0);
}
