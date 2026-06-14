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

/*
** philo.h
** -------
** Public header for the mandatory part of the Philosophers project.
** Declares:
**   - t_philo: per-philosopher state (one philosopher = one thread)
**   - t_data:  shared simulation state (forks, settings, stop flag)
**   - prototypes for every function used across the project.
**
** بالعربي:
** هذا هو الهيدر العام للجزء الإجباري من مشروع الفلاسفة.
** يحتوي على:
**   - t_philo: حالة كل فيلسوف (كل فيلسوف = ثريد منفصل).
**   - t_data:  الحالة المشتركة للمحاكاة (الشوك، الإعدادات، علم التوقف).
**   - تعريفات (prototypes) لجميع الدوال المستخدمة في المشروع.
*/

# include <pthread.h>   /* threads + mutexes / الثريدات والميوتيكس     */
# include <stdio.h>     /* printf for state messages / لطباعة الحالات */
# include <stdlib.h>    /* malloc, free / للحجز والتحرير              */
# include <sys/time.h>  /* gettimeofday for ms clock / للوقت بالملي    */
# include <unistd.h>    /* usleep, write / للنوم والكتابة             */

/*
** Forward declaration so that t_philo can hold a back-pointer to t_data
** before t_data itself is fully defined below.
**
** بالعربي:
** إعلان مُسبق (forward declaration) حتى يستطيع t_philo أن يحتفظ بمؤشر
** إلى t_data قبل أن يتم تعريف t_data بالكامل تحت.
*/
typedef struct s_data	t_data;

/*
** t_philo: one entry per philosopher (also one thread per philosopher).
**   id            : 1-based philosopher number (matches the subject).
**   meals_eaten   : how many full meals this philo has completed.
**   last_meal_ms  : absolute ms timestamp of the last time eating started.
**                   Used by the monitor to detect starvation.
**   thread        : the worker thread that runs philo_routine().
**   left_fork     : pointer to the mutex for the fork on this philo's left.
**   right_fork    : pointer to the mutex for the fork on this philo's right.
**   meal_lock     : protects meals_eaten and last_meal_ms (read by monitor,
**                   written by this philo).
**   data          : back-pointer to the shared simulation state.
**
** بالعربي:
** t_philo: عنصر واحد لكل فيلسوف (ولكل فيلسوف ثريد خاص به).
**   id            : رقم الفيلسوف ابتداءً من 1 (كما في الـ subject).
**   meals_eaten   : عدد الوجبات الكاملة التي أنهاها هذا الفيلسوف.
**   last_meal_ms  : وقت آخر بداية أكل بالملي ثانية (مطلق).
**                   يستخدمه المراقب للكشف عن الجوع.
**   thread        : الثريد العامل الذي يشغّل philo_routine().
**   left_fork     : مؤشر إلى الميوتكس الخاص بالشوكة اليسرى.
**   right_fork    : مؤشر إلى الميوتكس الخاص بالشوكة اليمنى.
**   meal_lock     : يحمي meals_eaten و last_meal_ms (يقرأ منه المراقب،
**                   ويكتب فيه هذا الفيلسوف).
**   data          : مؤشر عكسي إلى الحالة المشتركة للمحاكاة.
*/
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

/*
** t_data: simulation-wide shared state.
**   philo_count    : number of philosophers (and number of forks).
**   time_to_die    : ms a philo can go without eating before dying.
**   time_to_eat    : ms it takes to eat one meal (forks held the whole time).
**   time_to_sleep  : ms spent sleeping after a meal.
**   must_eat_count : optional argument; if >= 0, the simulation stops once
**                    every philo has eaten this many times. -1 means "off".
**   stop           : flag set to 1 by the monitor on death or completion.
**                    Read by every philo to know when to exit the loop.
**   start_ms       : absolute ms timestamp at simulation start; used to
**                    compute the relative timestamps shown in log lines.
**   monitor_thread : the watcher thread that polls for death / completion.
**   forks          : array of philo_count mutexes, one per fork.
**   print_lock     : serialises log output so messages never interleave.
**   stop_lock      : protects reads/writes of `stop`.
**   philos         : array of philo_count t_philo structs.
**
** بالعربي:
** t_data: الحالة المشتركة للمحاكاة كلها.
**   philo_count    : عدد الفلاسفة (وعدد الشوك أيضاً).
**   time_to_die    : عدد الملي ثانية اللي يقدر الفيلسوف يصمد من غير أكل.
**   time_to_eat    : مدة الأكل بالملي (الشوكتان ممسوكتان طوال هذه المدة).
**   time_to_sleep  : مدة النوم بعد كل وجبة.
**   must_eat_count : وسيط اختياري؛ إذا كان >= 0 تتوقف المحاكاة بعد ما
**                    يأكل كل فيلسوف هذا العدد من الوجبات. -1 = معطّل.
**   stop           : علم يضعه المراقب على 1 عند الوفاة أو الاكتمال.
**                    يقرأه كل فيلسوف ليعرف متى يخرج من حلقته.
**   start_ms       : وقت بداية المحاكاة (مطلق بالملي)؛ تُحسب منه
**                    الأوقات النسبية في كل سطر طباعة.
**   monitor_thread : ثريد المراقب الذي يفحص الوفاة والاكتمال.
**   forks          : مصفوفة ميوتيكس بحجم philo_count (شوكة لكل ميوتكس).
**   print_lock     : يحول دون تداخل أسطر الطباعة بين الثريدات.
**   stop_lock      : يحمي القراءة والكتابة على `stop`.
**   philos         : مصفوفة هياكل t_philo بحجم philo_count.
*/
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

/* parse.c: command-line argument validation. */
/* parse.c: التحقق من سلامة وسيطات سطر الأوامر. */
int		parse_args(t_data *data, int argc, char **argv);
/* parse.c: strict positive-integer parser (1 = success, 0 = fail). */
/* parse.c: محلّل أعداد صحيحة موجبة صارم (1 = نجاح، 0 = فشل).        */
int		ft_atoi_positive(const char *str, long *out);

/* init.c: allocates philos/forks arrays and inits all mutexes. */
/* init.c: يحجز مصفوفتي philos و forks ويهيئ كل الميوتيكس.         */
int		init_simulation(t_data *data);
/* init.c: destroys mutexes and frees the allocated arrays. */
/* init.c: يدمّر الميوتيكس ويحرّر المصفوفات المحجوزة.            */
void	cleanup_simulation(t_data *data);

/* sim.c: spawns philo threads + monitor, then joins everything. */
/* sim.c: يُنشئ ثريدات الفلاسفة وثريد المراقب ثم ينتظر انتهاءها.    */
int		start_simulation(t_data *data);

/* eat.c: one full meal cycle (take both forks, eat, release). */
/* eat.c: دورة وجبة كاملة (مسك الشوكتين، الأكل، ثم تركهما).       */
void	eat(t_philo *philo);

/* monitor.c: watcher thread entry (death + must-eat completion). */
/* monitor.c: نقطة دخول ثريد المراقب (الوفاة + إكمال الوجبات).     */
void	*monitor_routine(void *arg);

/* utils.c: time helpers and the shared stop-flag getters/setters. */
/* utils.c: دوال الوقت وقراءة/كتابة علم التوقف المشترك بأمان.         */
long	get_time_ms(void);
long	elapsed_ms(t_data *data);
void	precise_sleep(t_data *data, long duration_ms);
int		get_stop(t_data *data);
void	set_stop(t_data *data, int value);

/* log.c: prints a formatted state line under the print_lock. */
/* log.c: يطبع سطر حالة منسّقاً مع قفل الطباعة لتجنّب التداخل.     */
void	log_state(t_philo *philo, const char *msg);

#endif
