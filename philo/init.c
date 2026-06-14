/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** init.c
** ------
** Allocates the philos[] and forks[] arrays, initialises every mutex,
** wires each philosopher to its two neighbouring forks, and provides
** the symmetric cleanup routine (destroy + free).
**
** بالعربي:
** يحجز مصفوفتي philos[] و forks[]، ويهيئ كل ميوتيكس، ويربط كل فيلسوف
** بشوكتيه المجاورتين، ويوفّر دالة التنظيف المقابلة (تدمير + تحرير).
*/

/*
** init_mutexes
** ------------
** Initialises every mutex used by the simulation:
**   - print_lock: serialises stdout writes (one log line at a time).
**   - stop_lock:  protects the `stop` flag (read/written from many
**                 threads, so it cannot be a plain int access).
**   - forks[i]:   one mutex per fork on the table.
**   - philos[i].meal_lock: per-philosopher lock that guards
**                 last_meal_ms + meals_eaten (monitor reads, philo writes).
** Returns 0 on the first failure so the caller can bail out.
**
** بالعربي:
** يهيئ كل ميوتيكس تستخدمه المحاكاة:
**   - print_lock: يجعل الكتابة على stdout متتابعة (سطر واحد في كل مرة).
**   - stop_lock:  يحمي علم `stop` (تقرأه وتكتبه ثريدات كثيرة، فلا يصح
**                 الوصول إليه كقيمة int مباشرة).
**   - forks[i]:   ميوتكس واحد لكل شوكة على الطاولة.
**   - philos[i].meal_lock: قفل خاص بكل فيلسوف يحمي
**                 last_meal_ms و meals_eaten (المراقب يقرأ، الفيلسوف يكتب).
** يُعيد 0 عند أول فشل ليستطيع المستدعي الخروج بأمان.
*/
static int	init_mutexes(t_data *data)
{
	int	i;

	if (pthread_mutex_init(&data->print_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->stop_lock, NULL) != 0)
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

/*
** link_philos
** -----------
** Fills the per-philosopher fields. Each philosopher gets:
**   - id = index + 1 (subject uses 1-based numbering).
**   - left_fork = forks[i], right_fork = forks[(i + 1) % N], so the table
**     forms a ring and fork ownership wraps around between philo N and 1.
** Note: the deadlock-avoidance trick is NOT here -- it lives in eat.c,
** where the order in which the two forks are LOCKED depends on the
** philosopher's parity. Keeping left/right "natural" here makes the
** mental model simpler.
**
** بالعربي:
** يملأ حقول كل فيلسوف. كل فيلسوف يأخذ:
**   - id = index + 1 (الـ subject يستخدم ترقيماً يبدأ من 1).
**   - left_fork = forks[i]، right_fork = forks[(i + 1) % N]، بحيث تشكّل
**     الطاولة حلقة وتلتف ملكية الشوكة بين الفيلسوف N والفيلسوف 1.
** ملاحظة: حيلة تجنّب الـ deadlock ليست هنا، بل في eat.c، حيث يعتمد
** ترتيب قفل الشوكتين على فردية/زوجية رقم الفيلسوف. ترك left/right
** "طبيعيين" هنا يجعل التصوّر الذهني أبسط.
*/
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
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->philo_count];
		i++;
	}
}

/*
** init_simulation
** ---------------
** Top-level init entry point: allocate forks[] and philos[], then init
** every mutex and wire neighbours. Returns 0 on any failure -- the
** caller is then responsible for cleanup_simulation().
**
** بالعربي:
** نقطة الدخول الرئيسية للتهيئة: تحجز forks[] و philos[]، ثم تهيئ كل
** ميوتيكس وتربط الجيران. تُعيد 0 عند أي فشل، وعندها يكون من مسؤولية
** المستدعي تشغيل cleanup_simulation().
*/
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

/*
** cleanup_simulation
** ------------------
** Symmetric counterpart to init_simulation: destroys every mutex then
** frees the two heap arrays. Safe to call even after a partial init
** because main initialises forks/philos to NULL before parse.
**
** بالعربي:
** المقابل المتناظر لـ init_simulation: يدمّر كل ميوتيكس ثم يحرّر
** المصفوفتين. آمنة الاستدعاء حتى بعد تهيئة جزئية لأن main تجعل
** forks/philos = NULL قبل التحليل.
*/
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
	free(data->forks);
	free(data->philos);
}
