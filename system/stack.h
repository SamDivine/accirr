#ifndef _STACK_H
#define _STACK_H

class Worker;

typedef void (coro_func)(Worker *, void *arg);

void* swapstacks(void **olds, void **news, void *ret)
	asm("_swapstacks");


static inline void* swapstacks_inline(void **olds, void **news, void *ret) {
	asm volatile( "" : : : "memory" );
	return swapstacks(olds, news, ret);
}

void makestack(void **me, void **stack, coro_func, Worker *it)
	asm ("_makestack");

#endif //_STACK_H

