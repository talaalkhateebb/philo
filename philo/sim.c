#include "philo.h"

static void	single_philo_routine(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	log_state(philo, "has taken a fork");
	while (!get_stop(philo->data))
		usleep(500);
	pthread_mutex_unlock(philo->left_fork);
}

static void	eat(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	log_state(philo, "has taken a fork");
	pthread_mutex_lock(philo->right_fork);
	log_state(philo, "has taken a fork");
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_ms = get_time_ms();
	pthread_mutex_unlock(&philo->meal_lock);
	log_state(philo, "is eating");
	precise_sleep(philo->data, philo->data->time_to_eat);
	pthread_mutex_lock(&philo->meal_lock);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

static void	*philo_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->data->philo_count == 1)
		return (single_philo_routine(philo), NULL);
	if (philo->id % 2 == 0)
		usleep(500);
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
	}
	return (NULL);
}

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

static void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		i;
	long	now;
	long	last_meal;

	data = (t_data *)arg;
	while (!get_stop(data))
	{
		i = 0;
		while (i < data->philo_count && !get_stop(data))
		{
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
				return (NULL);
			}
			i++;
		}
		if (check_all_full(data))
			return (set_stop(data, 1), NULL);
		usleep(200);
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
