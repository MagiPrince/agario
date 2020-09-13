#include <pthread.h>
#include <stdio.h>

void create_thread(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);

void join_thread(pthread_t thread, void **value_ptr);

void init_mutex(pthread_mutex_t *lock);

void lock_mutex(pthread_mutex_t *mutex);

void unlock_mutex(pthread_mutex_t *mutex);

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

pthread_cond_t cond_init();

void cond_destroy(pthread_cond_t cond);

void cond_signal(pthread_cond_t *cond);

void barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count);

void barrier_wait(pthread_barrier_t *barrier);