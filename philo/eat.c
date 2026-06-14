/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eat.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** eat.c
** -----
** One full meal cycle for a philosopher. The deadlock-avoidance trick
** lives here: the ORDER in which a philo locks its two forks depends
** on its id parity, which breaks the symmetric circular wait that
** would otherwise lock the whole table.
**
** بالعربي:
** دورة وجبة كاملة لفيلسوف. حيلة تجنّب الـ deadlock تعيش هنا: ترتيب
** قفل الشوكتين يعتمد على فردية/زوجية رقم الفيلسوف، وهذا يكسر
** الانتظار الدائري المتناظر الذي سيُجمّد كل الطاولة لو حدث.
*/

/*
** take_forks
** ----------
** Picks up both forks in an id-dependent order:
**   - even-id philos lock RIGHT first, then LEFT.
**   - odd-id philos  lock LEFT  first, then RIGHT.
** Because adjacent philos have opposite parity (modulo the wrap), they
** reach for different "first" forks, so no two neighbours can both be
** holding one fork and waiting on the other (the classic deadlock).
** The two log lines are required by the subject: one per fork picked up.
**
** بالعربي:
** يلتقط الشوكتين بترتيب يعتمد على رقم الفيلسوف:
**   - الفلاسفة بأرقام زوجية يقفلون اليمنى أولاً ثم اليسرى.
**   - الفلاسفة بأرقام فردية يقفلون اليسرى أولاً ثم اليمنى.
** بما أن الجيران ذوو فردية/زوجية متعاكسة (مع التفاف الحلقة)، فإنهم
** يمدّون أيديهم إلى شوكة "أولى" مختلفة، فلا يمكن لجارين أن يمسك كل
** منهما شوكة وينتظر الأخرى (وهو الـ deadlock الكلاسيكي).
** السطران المطبوعان مطلوبان من الـ subject: واحد لكل شوكة تُلتقط.
*/
static void	take_forks(t_philo *philo, pthread_mutex_t **first,
		pthread_mutex_t **second)
{
	if (philo->id % 2 == 0)
	{
		*first = philo->right_fork;
		*second = philo->left_fork;
	}
	else
	{
		*first = philo->left_fork;
		*second = philo->right_fork;
	}
	pthread_mutex_lock(*first);
	log_state(philo, "has taken a fork");
	pthread_mutex_lock(*second);
	log_state(philo, "has taken a fork");
}

/*
** eat
** ---
** One full meal:
**   1. take_forks(): block until both forks are held.
**   2. Stamp last_meal_ms under meal_lock BEFORE logging "is eating",
**      so the monitor never sees an outdated timestamp and triggers a
**      false death.
**   3. Log "is eating" and sleep for time_to_eat (precise_sleep also
**      reacts to a mid-meal stop).
**   4. Bump meals_eaten under meal_lock so the monitor's all-fed check
**      stays consistent.
**   5. Release the forks in reverse order.
**
** بالعربي:
** وجبة كاملة واحدة:
**   1. take_forks(): يتم الحجب حتى تُمسك كلتا الشوكتين.
**   2. ختم last_meal_ms داخل meal_lock قبل طباعة "is eating"، حتى لا
**      يرى المراقب وقتاً قديماً ويُعلن وفاة كاذبة.
**   3. طباعة "is eating" والنوم لمدة time_to_eat (precise_sleep يستجيب
**      أيضاً لتوقف يحدث في منتصف الوجبة).
**   4. زيادة meals_eaten داخل meal_lock حتى يبقى فحص "كل الفلاسفة
**      شبعوا" متّسقاً.
**   5. تحرير الشوكتين بالترتيب العكسي.
*/
void	eat(t_philo *philo)
{
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;

	take_forks(philo, &first, &second);
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_ms = get_time_ms();
	pthread_mutex_unlock(&philo->meal_lock);
	log_state(philo, "is eating");
	precise_sleep(philo->data, philo->data->time_to_eat);
	pthread_mutex_lock(&philo->meal_lock);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
	pthread_mutex_unlock(second);
	pthread_mutex_unlock(first);
}
