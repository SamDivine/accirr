#ifndef _WORKER_H
#define _WORKER_H

#include "common.h"
#include "stack.h"

typedef uint32_t threadid_t;

class Scheduler;

class Worker {
public:
	Worker() {}
	
	void *stack;
	void *base;

	Worker *next;

	union {
		struct {
			int running : 1;
			int suspended : 1;
			int idle : 1;
		};
		int8_t run_state_raw_;
	};

	Scheduler *sched;
	bool done;

	size_t ssize;
	threadid_t id;

	Worker *tracking_prev;
	Worker *tracking_next;

	inline intptr_t stack_remaining() {
		register long rsp asm("rsp");

		int64_t remain = static_cast<int64_t>(rsp) - reinterpret_cast<int64_t>(this->base) - 4096;

		if (remain >= STACK_SIZE) {
			std::cerr << "stack_remaining GREATER THAN or EQUAL TO STACK_SIZE" << std::endl;
		} else if (remain < 0) {
			std::cerr << "stack_remaining LESS THAN 0" << std::endl;
		}

		return remain;
	}
} ACCIRR_BLOCK_ALIGNED;

void checked_mprotect( void *addr, size_t len, int prot );

typedef void (*thread_func)(Worker *, void *arg);

Worker *convert_to_master(Worker *me = NULL);

void coro_spawn(Worker *me, Worker *c, coro_func, size_t ssize);

static inline void *coro_invoke(Worker *me, Worker *to, void *val) {
	me->running = 0;
	to->running = 1;
	val = swapstacks_inline(&(me->stack), &(to->stack), val);
	return val;
}

void thread_exit(Worker *me, void *retval);

static void tramp(Worker *me, void *arg) {
	Worker *master = (Worker*)arg;
	Worker *my_thr = (Worker*)coro_invoke(me, master, NULL);
	thread_func f = (thread_func)coro_invoke(me, master, NULL);
	void *f_arg = coro_invoke(me, master, NULL);

	coro_invoke(me, master, NULL);

	//std::cerr << "f_arg =" << (intptr_t)f_arg << std::endl;
	f(my_thr, f_arg);

	thread_exit(my_thr, NULL);
}

Worker *worker_spawn(Worker *me, Scheduler *sched, thread_func f, void *arg);

void destroy_coro(Worker *c);

void destroy_thread(Worker *thr);

inline void *thread_context_switch(Worker *running, Worker *next, void *val) {
	void *res = coro_invoke(running, next, val);
	return res;
}

#endif //_WORKER_H

