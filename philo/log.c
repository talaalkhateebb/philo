#include "philo.h"

void	log_state(t_philo *philo, const char *msg)
{
	pthread_mutex_lock(&philo->data->print_lock);
	pthread_mutex_lock(&philo->data->stop_lock);
	if (!philo->data->stop)
		printf("%ld %d %s\n", elapsed_ms(philo->data), philo->id, msg);
	pthread_mutex_unlock(&philo->data->stop_lock);
	pthread_mutex_unlock(&philo->data->print_lock);
}
