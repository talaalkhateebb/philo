/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** parse.c
** -------
** Converts argv strings into typed numeric fields on t_data and validates
** that every required argument is a strictly positive integer that fits
** into INT_MAX. All errors are written to stderr; success returns 1.
**
** بالعربي:
** يحوّل سلاسل argv إلى حقول رقمية على t_data ويتحقق أن كل وسيط مطلوب
** هو عدد صحيح موجب تماماً ويناسب نطاق INT_MAX.
** كل الأخطاء تُكتب على stderr، والنجاح يُعيد 1.
*/

/*
** ft_atoi_positive
** ----------------
** Strict positive-integer parser. Unlike the libc atoi, this:
**   - rejects empty strings, signs ('+'/'-'), and any non-digit char.
**   - rejects overflow past INT_MAX (we cap at 2147483647 because the
**     subject says philo_count fits in an int).
** Returns 1 and writes the parsed value into *out on success, else 0.
**
** بالعربي:
** محلّل صارم لأعداد صحيحة موجبة. عكس atoi المعيارية:
**   - يرفض السلاسل الفارغة، والإشارات ('+'/'-')، وأي حرف غير رقم.
**   - يرفض الفيضان فوق INT_MAX (نحدّ عند 2147483647 لأن الـ subject
**     يقول إن philo_count يلزم أن يكون من نوع int).
** يعيد 1 ويكتب القيمة المحوّلة في *out عند النجاح، وإلا يعيد 0.
*/
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

/*
** validate_args
** -------------
** Post-parse sanity check on the numeric values:
**   - need at least one philosopher.
**   - all three time values must be >= 1 ms.
**   - if must_eat_count was explicitly set to 0, reject (a zero-meal run
**     would mean "stop immediately" which is nonsense for evaluation).
**
** بالعربي:
** فحص منطقية القيم الرقمية بعد التحليل:
**   - نحتاج على الأقل فيلسوفاً واحداً.
**   - كل قيم الوقت الثلاث يجب أن تكون >= 1 ملي ثانية.
**   - إذا حدّد المستخدم must_eat_count بـ 0 نرفض (لأن تشغيل بدون وجبات
**     لا معنى له في التقييم).
*/
static int	validate_args(t_data *data)
{
	if (data->philo_count < 1)
		return (0);
	if (data->time_to_die < 1 || data->time_to_eat < 1
		|| data->time_to_sleep < 1)
		return (0);
	if (data->must_eat_count == 0)
		return (0);
	return (1);
}

/*
** parse_args
** ----------
** Reads argv[1..4] (and optionally argv[5]) into t_data fields. Each
** field is parsed strictly; a specific error message is written for
** whichever argument failed. Finally validate_args enforces value ranges.
**
** بالعربي:
** تقرأ argv[1..4] (و argv[5] اختيارياً) إلى حقول t_data. كل حقل
** يُحلَّل بصرامة، وتُكتب رسالة خطأ محدّدة للوسيط الذي فشل.
** وأخيراً تتأكد validate_args من مجالات القيم.
*/
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
