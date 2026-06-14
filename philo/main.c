/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** main.c
** ------
** Program entry point. Validates argc, sets sane defaults on t_data,
** then runs the three pipeline stages: parse -> init -> simulate.
**
** بالعربي:
** نقطة دخول البرنامج. تتحقق من عدد الوسائط، تضبط قيماً ابتدائية
** آمنة في t_data، ثم تشغّل المراحل الثلاث: التحليل -> التهيئة -> المحاكاة.
*/

/*
** init_data_defaults
** ------------------
** Zero/initialise every field of t_data BEFORE parsing.
**   - must_eat_count = -1 means "not provided" (simulation only stops on
**     death). parse_args overwrites this only if argv[5] is given.
**   - forks/philos = NULL so cleanup is safe even if init fails halfway.
**
** بالعربي:
** يضبط كل حقول t_data إلى قيم ابتدائية قبل التحليل.
**   - must_eat_count = -1 معناها "لم تُمرَّر" (تتوقف المحاكاة عند الوفاة فقط).
**     سيغيّرها parse_args فقط إذا أعطى المستخدم argv[5].
**   - forks/philos = NULL ليكون استدعاء التنظيف آمناً حتى لو فشلت التهيئة
**     في منتصف الطريق.
*/
static void	init_data_defaults(t_data *data)
{
	data->philo_count = 0;
	data->time_to_die = 0;
	data->time_to_eat = 0;
	data->time_to_sleep = 0;
	data->must_eat_count = -1;
	data->stop = 0;
	data->start_ms = 0;
	data->forks = NULL;
	data->philos = NULL;
}

/*
** main
** ----
** Pipeline:
**   1. Check argc (4 mandatory args + optional 5th).
**   2. Parse arguments into the data struct.
**   3. Allocate arrays and initialise every mutex.
**   4. Spawn philosopher threads + monitor, then join them.
**   5. Always run cleanup_simulation to free memory and destroy mutexes.
**
** بالعربي:
** خط الإنتاج (Pipeline) لتشغيل البرنامج:
**   1. التحقق من argc (4 وسائط إجبارية + خامسة اختيارية).
**   2. تحليل الوسائط ووضعها في الهيكل t_data.
**   3. حجز المصفوفات وتهيئة كل ميوتيكس.
**   4. إطلاق ثريدات الفلاسفة وثريد المراقب ثم انتظار انتهائها.
**   5. تشغيل cleanup_simulation دائماً لتحرير الذاكرة وتدمير الميوتيكس.
*/
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
