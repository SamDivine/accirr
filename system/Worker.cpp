#include "stack.h"
#include "Worker.h"
#include "Scheduler.h"
#include <stdlib.h>

#ifdef CORO_DEBUG
Worker *all_coros = NULL;

size_t total_coros = 0;
#endif

int32_t stack_offset = 64;
size_t current_stack_offset = 0;

#ifdef CORO_DEBUG
void insert_coro(Worker *c) {
	if (all_coros) {
		all_coros->tracking_prev = c;
	}
	c->tracking_next = all_coros;
	all_coros = c;
}

void remove_coro(Worker *c) {
	if (c->tracking_prev) {
		c->tracking_prev->tracking_next = c->tracking_next;
	}
	if (c->tracking_next) {
		c->tracking_next->tracking_prev = c->tracking_prev;
	}
}
#endif

Worker *convert_to_master(Worker *me) {
	if (!me) {
#ifdef USING_MALLOC
		me = (Worker*)malloc(sizeof(Worker));
#else
		me = new Worker();
#endif
	}

	me->running = 1;
	me->suspended = 0;
	me->idle = 0;

	me->base = NULL;
	me->stack = NULL;

#ifdef CORO_DEBUG
	me->tracking_prev = NULL;
	me->tracking_next = NULL;

	total_coros++;
	insert_coro(me);
#endif
	me->sched = NULL;
	me->next = NULL;
	me->id = 0;
	me->done = false;

	return me;
}

void coro_spawn(Worker *me, Worker *c, coro_func f, size_t ssize) {
	if (!c) {
		std::cerr << "Must provide a valid Worker" << std::endl;
		exit(-1);
	}
	c->running = 0;
	c->suspended = 0;
	c->idle = 0;

	c->base = malloc(ssize+4096*2);
	c->ssize = ssize;

	c->stack = (char*)(c->base) + ssize + 4096 - current_stack_offset;

	const int num_offsets = 128;
	const int cache_line_size = 64;
	current_stack_offset += stack_offset;
	current_stack_offset &= ((cache_line_size * num_offsets) - 1);

#ifdef CORO_DEBUG
	c->tracking_prev = NULL;
	c->tracking_next = NULL;
#endif

	memset(c->base, 0, ssize);

	makestack(&(me->stack), &(c->stack), f, c);

#ifdef CORO_DEBUG
	insert_coro(c);
	total_coros++;
#endif
}

Worker *worker_spawn(Worker *me, Scheduler *sched, thread_func f, void *arg) {
#ifdef USING_MALLOC
	Worker *thr = (Worker*)malloc(sizeof(Worker));
#else
	Worker *thr = new Worker();
#endif
	thr->sched = sched;
	sched->assignTid(thr);
	sched->workerNumInc();

	coro_spawn(me, thr, tramp, STACK_SIZE);

	coro_invoke(me, thr, (void*)me);
	coro_invoke(me, thr, thr);
	coro_invoke(me, thr, (void*)f);
	coro_invoke(me, thr, (void*)arg);

	//std::cerr << "arg=" << (intptr_t)arg << std::endl;

	return thr;
}

void destroy_coro(Worker *c) {
#ifdef CORO_DEBUG
	total_coros--;
#endif
	if (c->base != NULL) {
#ifdef CORO_DEBUG
		remove_coro(c);
#endif
		free(c->base);
	}
}

void destroy_thread(Worker *thr) {
	destroy_coro(thr);
	free(thr);
}

void thread_exit(Worker *me, void *retval) {
	me->next = (Worker*)retval;

	me->sched->workerNumDec();
	me->sched->thread_on_exit();
	exit(EXIT_FAILURE);
}


