#pragma once

#include <pthread.h>

typedef struct {
	int readers;
	int writer;
	pthread_cond_t readers_proceed;
	pthread_cond_t writer_proceed;
	int pending_writers;
	pthread_mutex_t read_write_lock;
} rwlock_t;

void rwlock_init(rwlock_t *l);
void rwlock_rlock(rwlock_t *l);
void rwlock_wlock(rwlock_t *l);
void rwlock_unlock(rwlock_t *l);
