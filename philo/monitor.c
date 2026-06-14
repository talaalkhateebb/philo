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

/*
** monitor.c
** ---------
** The watcher thread. While the simulation is alive it loops over every
** philosopher checking two end conditions:
**   1. Did anyone go too long without a meal? -> announce death, stop.
**   2. Have all philos hit must_eat_count meals? -> stop silently.
** The 200us poll interval keeps the death-detection latency well under
** the 10 ms upper bound required by the subject.
**
** بالعربي:
** ثريد المراقب. ما دامت المحاكاة حيّة، يدور على كل فيلسوف ويفحص
** شرطين للنهاية:
**   1. هل تأخر أحدهم في الأكل أكثر من اللازم؟ -> أعلن الوفاة وأوقف.
**   2. هل بلغ كل الفلاسفة must_eat_count من الوجبات؟ -> أوقف بصمت.
** فترة الفحص 200us تجعل تأخر اكتشاف الوفاة أقل بكثير من حدّ الـ 10
** ملي المطلوب في الـ subject.
*/

/*
** check_all_full
** --------------
** Returns 1 only when must_eat_count >= 0 AND every philosopher has
** completed at least that many meals. We grab each meal_lock briefly
** so we never tear-read the meals_eaten field while a philo is updating
** it after a meal.
**
** بالعربي:
** تُعيد 1 فقط عندما يكون must_eat_count >= 0 وكل فيلسوف أنهى على
** الأقل ذلك العدد من الوجبات. نأخذ كل meal_lock لفترة قصيرة حتى لا
** نقرأ meals_eaten قراءة ممزّقة أثناء تحديث فيلسوف لها بعد وجبة.
*/
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

/*
** check_death
** -----------
** Checks one philosopher for starvation:
**   - reads last_meal_ms under meal_lock (avoids a torn long read).
**   - if (now - last_meal) >= time_to_die, sets stop = 1 AND prints
**     the death line atomically: print_lock + stop_lock are held
**     together so no other philo can squeeze a stale log line in
**     between us setting stop and printing the "died" message.
** Returns 1 when a death was announced (caller should exit), else 0.
**
** بالعربي:
** يفحص فيلسوفاً واحداً للجوع:
**   - يقرأ last_meal_ms داخل meal_lock (لتفادي قراءة long ممزّقة).
**   - إذا كان (now - last_meal) >= time_to_die، يضع stop = 1 ويطبع
**     سطر الوفاة بشكل ذري: نمسك print_lock + stop_lock معاً حتى لا
**     يتمكن أي فيلسوف آخر من إقحام سطر قديم بين ضبطنا لـ stop
**     وطباعة رسالة "died".
** يُعيد 1 إذا أُعلنت وفاة (وعندها يخرج المستدعي)، وإلا 0.
*/
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

/*
** monitor_routine
** ---------------
** Watcher thread entry. Polls every 200us:
**   - scan every philo via check_death; bail out as soon as one dies.
**   - if no death, check whether everyone is full and stop silently.
** Returns when stop has been set (either by us or by check_all_full).
**
** بالعربي:
** نقطة دخول ثريد المراقب. يفحص كل 200us:
**   - يمر على كل فيلسوف عبر check_death؛ يخرج فور موت واحد.
**   - إذا لم يحدث موت، يفحص هل شبع الجميع، فإذا نعم يتوقف بصمت.
** يعود عندما يتم ضبط stop (سواءً منّا أو من check_all_full).
*/
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
