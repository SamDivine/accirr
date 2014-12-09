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

#ifndef STACK_SIZE
#define STACK_SIZE (1<<13)
#endif

#ifndef BLOCK_SIZE
#define BLOCK_SIZE sizeof(int64_t)*8
#endif

#ifndef ACCIRR_BLOCK_ALIGNED
#define ACCIRR_BLOCK_ALIGNED __attribute__((aligned(BLOCK_SIZE)))
#endif

#define WORKER_PREFETCH
//#define STACK_PREFETCH

#ifndef WORKER_PREFETCH_LOCALITY
#define WORKER_PREFETCH_LOCALITY 0
#endif

#ifndef STACK_PREFETCH_LOCALITY
#define STACK_PREFETCH_LOCALITY 3
#endif

#ifndef READYQ_PREFETCH_DISTANCE
#define READYQ_PREFETCH_DISTANCE 4
#endif

#endif //_COMMON_H

