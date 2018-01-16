/*
 Modified `timer.h` from Development Kit Lab1 (from eClass)
 Macro for time measuring.
 */

#pragma once

#include <sys/time.h>

#define GET_TIME(now) { \
	struct timeval t; \
	gettimeofday(&t, NULL); \
	now = t.tv_sec + t.tv_usec/1000000.0; \
}
