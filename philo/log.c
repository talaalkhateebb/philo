/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: talkhati <talkhati@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 12:00:00 by talkhati         #+#    #+#             */
/*   Updated: 2026/06/09 12:00:00 by talkhati         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** log.c
** -----
** Single function that prints one state line for one philosopher.
**
** بالعربي:
** دالة واحدة تطبع سطر حالة واحد لفيلسوف واحد.
*/

/*
** log_state
** ---------
** Formats one log line: "<elapsed_ms> <philo_id> <msg>". Two locks are
** needed:
**   - print_lock: serialises stdout so two threads never interleave
**     output bytes within a single line.
**   - stop_lock:  prevents printing AFTER the monitor has set stop = 1
**     and printed the "died" line. Without this guard a philosopher
**     could print "is sleeping" at, say, ts+1 after the monitor printed
**     "ts X died", which the evaluator rejects.
**
** بالعربي:
** تنسّق سطراً واحداً بالشكل: "<elapsed_ms> <philo_id> <msg>".
** نحتاج إلى قفلين:
**   - print_lock: لجعل الكتابة على stdout متتابعة فلا تتداخل بايتات
**     ثريدين داخل نفس السطر.
**   - stop_lock:  لمنع الطباعة بعد أن يضع المراقب stop = 1 ويطبع سطر
**     "died". بدون هذا الحارس قد يطبع فيلسوف "is sleeping" عند ts+1
**     بعد أن طبع المراقب "ts X died"، وهذا يرفضه المُقيِّم.
*/
void	log_state(t_philo *philo, const char *msg)
{
	pthread_mutex_lock(&philo->data->print_lock);
	pthread_mutex_lock(&philo->data->stop_lock);
	if (!philo->data->stop)
		printf("%ld %d %s\n", elapsed_ms(philo->data), philo->id, msg);
	pthread_mutex_unlock(&philo->data->stop_lock);
	pthread_mutex_unlock(&philo->data->print_lock);
}
