/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** utils.c
** -------
** Time helpers, the thread-safe stop-flag accessors, and a precise
** "sleep that also wakes up on stop" helper used during eat/sleep phases.
**
** بالعربي:
** دوال مساعدة للوقت، ودوال آمنة بين الثريدات لقراءة/كتابة علم التوقف،
** ودالة نوم دقيقة تستيقظ مبكراً عند ضبط علم التوقف خلال الأكل أو النوم.
*/

/*
** get_time_ms
** -----------
** Returns the current absolute time in milliseconds since the Unix
** epoch. We use gettimeofday because the subject's allowed-functions
** list excludes clock_gettime. Sub-millisecond precision (us / 1000)
** is enough -- the subject requires only millisecond-resolution logs.
**
** بالعربي:
** تُعيد الوقت الحالي المطلق بالملي ثانية منذ بداية حقبة Unix.
** نستخدم gettimeofday لأن قائمة الدوال المسموحة في الـ subject لا
** تتضمن clock_gettime. دقة دون الملي (us / 1000) كافية لأن الـ subject
** يطلب فقط طباعة بالميلي ثانية.
*/
long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/*
** elapsed_ms
** ----------
** Returns "ms since simulation start". Used to print the relative
** timestamps that appear at the beginning of every log line.
**
** بالعربي:
** تُعيد "الملي ثانية منذ بدء المحاكاة". تُستخدم لطباعة الوقت النسبي
** الذي يظهر في بداية كل سطر طباعة.
*/
long	elapsed_ms(t_data *data)
{
	return (get_time_ms() - data->start_ms);
}

/*
** get_stop
** --------
** Thread-safe read of data->stop. Without the lock this would be a
** data race (one thread writes, others read) which the subject
** explicitly forbids.
**
** بالعربي:
** قراءة آمنة بين الثريدات لـ data->stop. بدون القفل ستصبح هذه قراءة
** سباقية (Data race): ثريد يكتب وآخرون يقرؤون، وهذا ممنوع صراحة في
** الـ subject.
*/
int	get_stop(t_data *data)
{
	int	value;

	pthread_mutex_lock(&data->stop_lock);
	value = data->stop;
	pthread_mutex_unlock(&data->stop_lock);
	return (value);
}

/*
** set_stop
** --------
** Thread-safe write of data->stop. Called by the monitor when a philo
** dies or when must_eat_count has been reached by everyone.
**
** بالعربي:
** كتابة آمنة بين الثريدات لـ data->stop. يستدعيها المراقب عند موت
** أحد الفلاسفة أو عند بلوغ كل الفلاسفة must_eat_count من الوجبات.
*/
void	set_stop(t_data *data, int value)
{
	pthread_mutex_lock(&data->stop_lock);
	data->stop = value;
	pthread_mutex_unlock(&data->stop_lock);
}

/*
** precise_sleep
** -------------
** Sleeps for `duration_ms` milliseconds, but in 200us increments so we
** can break out early if the simulation is stopping. Plain usleep(N)
** would make a dying philosopher print stale "is sleeping" / "is
** thinking" lines after the monitor has already announced the death.
**
** بالعربي:
** تنام لفترة `duration_ms` ملي ثانية، لكن على مراحل صغيرة 200us حتى
** نستطيع الخروج مبكراً إذا توقفت المحاكاة. لو استخدمنا usleep(N) مباشرة
** فإن الفيلسوف على وشك الموت قد يطبع أسطر "is sleeping" أو "is thinking"
** قديمة بعد أن أعلن المراقب وفاته.
*/
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
