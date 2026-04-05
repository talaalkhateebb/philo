#include "philo.h"

static void	init_data_defaults(t_data *data)
{
	data->philo_count = 0;
	data->time_to_die = 0;
	data->time_to_eat = 0;
	data->time_to_sleep = 0;
	data->must_eat_count = -1;
	data->stop = 0;
	data->start_ms = 0;
	data->finished_count = 0;
	data->forks = NULL;
	data->philos = NULL;
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (argc != 5 && argc != 6)
	{
		write(2, "Usage: ./philo n t_die t_eat t_sleep [must_eat]\n", 49);
		return (1);
	}
	init_data_defaults(&data);
	if (!parse_args(&data, argc, argv))
		return (1);
	if (!init_simulation(&data))
		return (1);
	if (!start_simulation(&data))
	{
		cleanup_simulation(&data);
		return (1);
	}
	cleanup_simulation(&data);
	return (0);
}
