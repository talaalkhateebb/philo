/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	check_all_full(t_data *data)
{
	int	i;
	int	full;

	if (data->must_eat_count < 0)
		return (0);
	i = 0;
	full = 0;
	while (i < data->philo_count)
	{
		pthread_mutex_lock(&data->philos[i].meal_lock);
		if (data->philos[i].meals_eaten >= data->must_eat_count)
			full++;
		pthread_mutex_unlock(&data->philos[i].meal_lock);
		i++;
	}
	return (full == data->philo_count);
}

static int	check_death(t_data *data, int i)
{
	long	now;
	long	last_meal;

	pthread_mutex_lock(&data->philos[i].meal_lock);
	last_meal = data->philos[i].last_meal_ms;
	pthread_mutex_unlock(&data->philos[i].meal_lock);
	now = get_time_ms();
	if (now - last_meal >= data->time_to_die)
	{
		pthread_mutex_lock(&data->print_lock);
		pthread_mutex_lock(&data->stop_lock);
		data->stop = 1;
		printf("%ld %d died\n", elapsed_ms(data), data->philos[i].id);
		pthread_mutex_unlock(&data->stop_lock);
		pthread_mutex_unlock(&data->print_lock);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		i;

	data = (t_data *)arg;
	while (!get_stop(data))
	{
		i = 0;
		while (i < data->philo_count && !get_stop(data))
		{
			if (check_death(data, i))
				return (NULL);
			i++;
		}
		if (check_all_full(data))
			return (set_stop(data, 1), NULL);
		usleep(200);
	}
	return (NULL);
}
