#include "thread.h"

void create_thread(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg) {
    if(pthread_create(thread, attr, start_routine, arg)){
        perror("Error at the creation of the thread");
    }
}

void join_thread(pthread_t thread, void **value_ptr) {
    if(pthread_join(thread, value_ptr)){
        perror("Error at the join thread");
    }
}

void init_mutex(pthread_mutex_t *lock){
    if(pthread_mutex_init(lock, NULL)){
        perror("Can't initialize mutex");
    }
}

void lock_mutex(pthread_mutex_t *mutex) {
    if(pthread_mutex_lock(mutex)){
        perror("Error at mutex lock\n");
    }
}

void unlock_mutex(pthread_mutex_t *mutex) {
    if(pthread_mutex_unlock(mutex)){
        perror("Error at mutex unlock\n");
    }
}

void destroy_mutex(pthread_mutex_t *mutex) {
    if(pthread_mutex_destroy(mutex)){
        perror("Error at destruction of mutex");
    }
}

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
    if(pthread_cond_wait(cond, mutex)){
        perror("Error at cond_wait");
    }
}

pthread_cond_t cond_init(){

    pthread_cond_t cond;
    if(pthread_cond_init(&cond, NULL)){
        perror("Error at initiation of cond");
    }
    return cond;
}

void cond_destroy(pthread_cond_t cond){
    if(pthread_cond_destroy(&cond)){
        perror("Error at destruction of cond");
    }
}

void cond_signal(pthread_cond_t *cond){
    if(pthread_cond_signal(cond)){
        perror("Error at cond signal");
    }
}

void barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count){
    if(pthread_barrier_init(barrier, attr, count)){
        perror("Error at init of barrier");
    }
}

void barrier_wait(pthread_barrier_t *barrier){
    int res = pthread_barrier_wait(barrier);
    if(res != 0 && res != PTHREAD_BARRIER_SERIAL_THREAD){
        perror("Error at barrier wait");
    }
}