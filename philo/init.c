#include "philo.h"

static int	init_mutexes(t_data *data)
{
	int	i;

	if (pthread_mutex_init(&data->print_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->stop_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->finish_lock, NULL) != 0)
		return (0);
	i = 0;
	while (i < data->philo_count)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
			return (0);
		if (pthread_mutex_init(&data->philos[i].meal_lock, NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

static void	link_philos(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->philo_count)
	{
		data->philos[i].id = i + 1;
		data->philos[i].meals_eaten = 0;
		data->philos[i].last_meal_ms = 0;
		data->philos[i].data = data;
		if (i % 2 == 0)
		{
			data->philos[i].left_fork = &data->forks[i];
			data->philos[i].right_fork = &data->forks[(i + 1) % data->philo_count];
		}
		else
		{
			data->philos[i].left_fork = &data->forks[(i + 1) % data->philo_count];
			data->philos[i].right_fork = &data->forks[i];
		}
		i++;
	}
}

int	init_simulation(t_data *data)
{
	data->forks = malloc(sizeof(pthread_mutex_t) * data->philo_count);
	data->philos = malloc(sizeof(t_philo) * data->philo_count);
	if (!data->forks || !data->philos)
		return (write(2, "Allocation failed\n", 18), 0);
	if (!init_mutexes(data))
		return (write(2, "Mutex initialization failed\n", 28), 0);
	link_philos(data);
	return (1);
}

void	cleanup_simulation(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->philo_count)
	{
		pthread_mutex_destroy(&data->forks[i]);
		pthread_mutex_destroy(&data->philos[i].meal_lock);
		i++;
	}
	pthread_mutex_destroy(&data->print_lock);
	pthread_mutex_destroy(&data->stop_lock);
	pthread_mutex_destroy(&data->finish_lock);
	free(data->forks);
	free(data->philos);
}
