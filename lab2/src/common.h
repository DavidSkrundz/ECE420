#pragma once

/// Required to compile on linux
#if defined(__linux__)
	#define _POSIX_C_SOURCE 1
#endif

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

char* localhost;

void fixOpenFileLimit();
void createSemaphore(sem_t** semaphore, char name[]);
void destroySemaphore(sem_t* semaphore, char name[]);
