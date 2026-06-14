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

/*
** sim.c
** -----
** The per-philosopher thread routine and the top-level start_simulation
** that spawns + joins everything. The actual eating logic lives in
** eat.c; the watcher lives in monitor.c.
**
** بالعربي:
** روتين الثريد لكل فيلسوف بالإضافة إلى start_simulation التي تُنشئ
** كل الثريدات وتنتظر انتهاءها. منطق الأكل الفعلي في eat.c، وثريد
** المراقب في monitor.c.
*/

/*
** single_philo_routine
** --------------------
** Special-case for philo_count == 1. With only one fork on the table,
** the philosopher can never grab two -- the subject expects them to
** pick up the single fork, fail to eat, and die after time_to_die.
** We log "has taken a fork", busy-wait until the monitor sets stop,
** then release the fork so cleanup can destroy the mutex cleanly.
**
** بالعربي:
** حالة خاصة عندما يكون philo_count == 1. بشوكة واحدة فقط على الطاولة
** لا يستطيع الفيلسوف الإمساك باثنتين، والـ subject يتوقع أن يلتقط
** الشوكة الوحيدة، يفشل في الأكل، ثم يموت بعد time_to_die.
** نطبع "has taken a fork"، ثم ننتظر بشكل نشط حتى يضع المراقب stop،
** ثم نحرّر الشوكة لتقدر دالة التنظيف على تدمير الميوتكس بسلام.
*/
static void	single_philo_routine(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	log_state(philo, "has taken a fork");
	while (!get_stop(philo->data))
		usleep(500);
	pthread_mutex_unlock(philo->left_fork);
}

/*
** think_pause_ms
** --------------
** Computes an extra "thinking" delay inserted between sleep and the
** next attempt to eat. Rationale: when time_to_die is much larger than
** time_to_eat + time_to_sleep, philos race back to the forks the moment
** they wake up, starving the neighbour that's still waiting. Adding a
** half-of-the-slack pause spreads fork usage and keeps everyone fed.
** Capped at 200 ms so we never sleep past a realistic eat cycle.
**
** بالعربي:
** تحسب تأخيراً إضافياً للـ "تفكير" يُدخَل بين النوم ومحاولة الأكل
** التالية. السبب: عندما يكون time_to_die أكبر بكثير من
** time_to_eat + time_to_sleep، يتسابق الفلاسفة على الشوك فور
** استيقاظهم فيُجوّعون الجار الذي ما زال ينتظر. إضافة فترة بنصف
** الفائض توزّع استخدام الشوك ويُبقي الجميع شبعاناً.
** نحدّ القيمة عند 200 ملي لئلا نتجاوز دورة أكل واقعية.
*/
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

/*
** philo_routine
** -------------
** Thread entry for each philosopher.
**   1. If only one philo exists, hand off to the special case.
**   2. Even-id philosophers wait `time_to_eat` ms before starting. This
**      stagger lets the odd-id philos grab their forks first, halving
**      the contention so neighbours don't deadlock-loop on each other.
**   3. Main loop: eat -> sleep -> think (-> optional pause), checking
**      the stop flag between every phase so we can exit promptly.
**
** بالعربي:
** نقطة دخول الثريد لكل فيلسوف.
**   1. إذا كان هناك فيلسوف واحد فقط، نسلّم الأمر إلى الحالة الخاصة.
**   2. الفلاسفة بأرقام زوجية ينتظرون `time_to_eat` ملي قبل البدء.
**      هذا التأخير يسمح للفلاسفة الفرديين بمسك شوكهم أولاً، فيقلّ
**      التنافس وتزول حلقة الـ deadlock بين الجيران.
**   3. الحلقة الرئيسية: أكل -> نوم -> تفكير (-> توقف اختياري)،
**      ونفحص علم التوقف بين كل مرحلة لنخرج بسرعة عند الحاجة.
*/
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

/*
** start_simulation
** ----------------
** Top-level orchestrator:
**   1. Record start_ms and seed each philo's last_meal_ms = start_ms so
**      the monitor measures starvation from t = 0.
**   2. Spawn one thread per philo running philo_routine.
**   3. Spawn the monitor thread.
**   4. Wait for the monitor to return (death or all-fed), then join
**      every philo thread so we have no detached workers at cleanup.
**
** بالعربي:
** المنسّق الرئيسي:
**   1. تسجيل start_ms وضبط last_meal_ms = start_ms لكل فيلسوف حتى
**      يقيس المراقب الجوع منذ اللحظة 0.
**   2. إنشاء ثريد لكل فيلسوف يعمل بـ philo_routine.
**   3. إنشاء ثريد المراقب.
**   4. انتظار عودة المراقب (إما لوفاة أو لاكتمال الوجبات)، ثم
**      انتظار كل ثريدات الفلاسفة (join) حتى لا يبقى ثريد طليق
**      عند التنظيف.
*/
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
