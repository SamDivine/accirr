#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/time.h>

#define WEBUI

#ifdef WEBUI
#include <sqlite3.h>
#endif

#ifndef STACK_SIZE
#define STACK_SIZE (1<<15)
#endif

#ifndef BLOCK_SIZE
#define BLOCK_SIZE sizeof(int64_t)*8
#endif

#ifndef ACCIRR_BLOCK_ALIGNED
#define ACCIRR_BLOCK_ALIGNED __attribute__((aligned(BLOCK_SIZE)))
#endif

#define WORKER_PREFETCH
#define STACK_PREFETCH

#ifndef READYQ_PREFETCH_DISTANCE
#define READYQ_PREFETCH_DISTANCE 4
#endif

struct timeval jobStart, jobNow;
struct timeval taskStart, taskNow;

double nsec(const timeval& start, timeval& end) {
	gettimeofday(&end, NULL);
	return (end.tv_sec-start.tv_usec)+(end.tv_usec-start.tv_usec)/1000000.0;
}


#endif //_COMMON_H

