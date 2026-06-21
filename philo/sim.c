/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	single_philo_routine(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	log_state(philo, "has taken a fork");
	while (!get_stop(philo->data))
		usleep(500);
	pthread_mutex_unlock(philo->left_fork);
}

static long	think_pause_ms(t_data *data)
{
	long	pause;

	pause = data->time_to_die - data->time_to_eat - data->time_to_sleep;
	if (pause < 0)
		pause = 0;
	pause /= 2;
	if (pause > 200)
		pause = 200;
	return (pause);
}

static void	*philo_routine(void *arg)
{
	t_philo	*philo;
	long	pause;

	philo = (t_philo *)arg;
	if (philo->data->philo_count == 1)
		return (single_philo_routine(philo), NULL);
	pause = think_pause_ms(philo->data);
	if (philo->id % 2 == 0)
		usleep(philo->data->time_to_eat * 1000);
	while (!get_stop(philo->data))
	{
		eat(philo);
		if (get_stop(philo->data))
			break ;
		log_state(philo, "is sleeping");
		precise_sleep(philo->data, philo->data->time_to_sleep);
		if (get_stop(philo->data))
			break ;
		log_state(philo, "is thinking");
		if (pause > 0)
			precise_sleep(philo->data, pause);
	}
	return (NULL);
}

int	start_simulation(t_data *data)
{
	int	i;

	data->start_ms = get_time_ms();
	i = 0;
	while (i < data->philo_count)
	{
		data->philos[i].last_meal_ms = data->start_ms;
		if (pthread_create(&data->philos[i].thread, NULL, philo_routine,
				&data->philos[i]) != 0)
			return (write(2, "Thread creation failed\n", 23), 0);
		i++;
	}
	if (pthread_create(&data->monitor_thread, NULL, monitor_routine, data) != 0)
		return (write(2, "Monitor thread creation failed\n", 31), 0);
	pthread_join(data->monitor_thread, NULL);
	i = 0;
	while (i < data->philo_count)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
	return (1);
}
