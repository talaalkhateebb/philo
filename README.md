*"This project has been created as part of the 42 curriculum by talkhati."*

## Description

This project implements the classic Dining Philosophers concurrency problem (phil.42).
It simulates philosophers who repeatedly take forks, eat, sleep, and think, while a
monitor detects starvation and stops the simulation when a philosopher dies (or when
all philosophers have eaten a required number of times, if provided).

Mandatory part (`philo/`) uses POSIX threads and mutexes:

- One philosopher = one thread
- One fork = one mutex
- Output is serialized (no overlapping messages)
- No global variables are used

## Instructions

### Compile

From the project root:

`make -C philo`

This builds `philo/philo`.

### Run

`./philo/philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]`

Example:

`./philo/philo 5 800 200 200`

## Resources

- Dining Philosophers: [https://en.wikipedia.org/wiki/Dining_philosophers_problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- POSIX Threads (pthreads): [https://man7.org/linux/man-pages/man7/pthreads.7.html](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- Mutexes: `pthread_mutex_init`, `pthread_mutex_lock`, `pthread_mutex_unlock`

### AI usage (disclosure)

AI assistance was used to:

- Outline a minimal architecture for `t_data` / `t_philo` without globals
- Draft thread routines and a monitor loop for death detection
- Draft a Makefile skeleton

All code was then reviewed and adjusted to respect the project's allowed functions
and output/timing constraints.
