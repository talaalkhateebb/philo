/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_data	t_data;

typedef struct s_philo
{
	int				id;
	int				meals_eaten;
	long			last_meal_ms;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	meal_lock;
	t_data			*data;
}	t_philo;

struct s_data
{
	int				philo_count;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				must_eat_count;
	int				stop;
	long			start_ms;
	pthread_t		monitor_thread;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	stop_lock;
	t_philo			*philos;
};

int		parse_args(t_data *data, int argc, char **argv);

int		ft_atoi_positive(const char *str, long *out);

int		init_simulation(t_data *data);

void	cleanup_simulation(t_data *data);

int		start_simulation(t_data *data);

void	eat(t_philo *philo);

void	*monitor_routine(void *arg);

long	get_time_ms(void);
long	elapsed_ms(t_data *data);
void	precise_sleep(t_data *data, long duration_ms);
int		get_stop(t_data *data);
void	set_stop(t_data *data, int value);

void	log_state(t_philo *philo, const char *msg);

#endif
