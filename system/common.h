#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

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

#endif //_COMMON_H

