#include "philo.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

long	elapsed_ms(t_data *data)
{
	return (get_time_ms() - data->start_ms);
}

int	get_stop(t_data *data)
{
	int	value;

	pthread_mutex_lock(&data->stop_lock);
	value = data->stop;
	pthread_mutex_unlock(&data->stop_lock);
	return (value);
}

void	set_stop(t_data *data, int value)
{
	pthread_mutex_lock(&data->stop_lock);
	data->stop = value;
	pthread_mutex_unlock(&data->stop_lock);
}

void	precise_sleep(t_data *data, long duration_ms)
{
	long	start;

	start = get_time_ms();
	while (!get_stop(data))
	{
		if (get_time_ms() - start >= duration_ms)
			break ;
		usleep(200);
	}
}
