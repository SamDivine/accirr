#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#define WEBUI

#ifdef WEBUI
#include <sqlite3.h>
#endif

#ifndef STACK_SIZE
#define STACK_SIZE (1<<14)
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
#define READYQ_PREFETCH_DISTANCE 8
#endif

#endif //_COMMON_H

