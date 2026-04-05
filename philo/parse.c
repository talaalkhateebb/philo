#include "philo.h"

int	ft_atoi_positive(const char *str, long *out)
{
	long	result;
	int		i;

	result = 0;
	i = 0;
	if (!str || !str[0])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		result = (result * 10) + (str[i] - '0');
		if (result > 2147483647)
			return (0);
		i++;
	}
	*out = result;
	return (1);
}

static int	validate_args(t_data *data)
{
	if (data->philo_count < 1)
		return (0);
	if (data->time_to_die < 1 || data->time_to_eat < 1 || data->time_to_sleep < 1)
		return (0);
	if (data->must_eat_count == 0)
		return (0);
	return (1);
}

int	parse_args(t_data *data, int argc, char **argv)
{
	long	value;

	if (!ft_atoi_positive(argv[1], &value))
		return (write(2, "Invalid number_of_philosophers\n", 31), 0);
	data->philo_count = (int)value;
	if (!ft_atoi_positive(argv[2], &value))
		return (write(2, "Invalid time_to_die\n", 20), 0);
	data->time_to_die = value;
	if (!ft_atoi_positive(argv[3], &value))
		return (write(2, "Invalid time_to_eat\n", 20), 0);
	data->time_to_eat = value;
	if (!ft_atoi_positive(argv[4], &value))
		return (write(2, "Invalid time_to_sleep\n", 22), 0);
	data->time_to_sleep = value;
	if (argc == 6)
	{
		if (!ft_atoi_positive(argv[5], &value))
			return (write(2, "Invalid must_eat_count\n", 23), 0);
		data->must_eat_count = (int)value;
	}
	if (!validate_args(data))
		return (write(2, "Arguments must be positive integers\n", 36), 0);
	return (1);
}
