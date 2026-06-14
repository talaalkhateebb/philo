*This project has been created as part of the 42 curriculum by talkhati.*

## Description

This project implements the mandatory part of the Philosophers assignment (philosophers
with threads and mutexes). Each philosopher runs in its own thread, each fork is
protected by a mutex, and a monitor thread detects starvation.

The simulation stops when a philosopher dies, or when every philosopher has eaten the
required number of times if the optional fifth argument is provided.

## Instructions

### Compilation

```bash
cd philo
make
```

Required Makefile rules: `all`, `clean`, `fclean`, and `re`.

### Execution

```bash
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

Example:

```bash
./philo 5 800 200 200
./philo 5 800 200 200 7
```

## Resources

- [Dining philosophers problem (Wikipedia)](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [POSIX threads (man7.org)](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [pthread_mutex_init (man7.org)](https://man7.org/linux/man-pages/man3/pthread_mutex_init.3.html)

### AI usage

AI was used to outline the initial architecture (`t_data` / `t_philo`), draft the monitor
loop, and draft the Makefile skeleton. All generated code was reviewed, tested, and
adjusted to match the subject constraints.
