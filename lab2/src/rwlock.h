#pragma once

#include <pthread.h>

typedef struct {
    int readers;
    int writer;
    pthread_cond_t readers_proceed;
    pthread_cond_t writer_proceed;
    int pending_writers;
    pthread_mutex_t read_write_lock;
} mylib_rwlock_t;

void mylib_rwlock_init (mylib_rwlock_t *l);
void mylib_rwlock_rlock(mylib_rwlock_t *l);
void mylib_rwlock_wlock(mylib_rwlock_t *l);
void mylib_rwlock_unlock(mylib_rwlock_t *l);
