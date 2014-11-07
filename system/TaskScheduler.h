#ifndef _TASK_SCHEDULER_H
#define _TASK_SCHEDULER_H

#include "common.h"
#include "Worker.h"
#include "ThreadQueue.h"
#include "Scheduler.h"

static uint64_t scheduler_context_switches = 0;
static uint64_t scheduler_count = 0;

class TaskScheduler : public Scheduler {
private:
	PrefetchingThreadQueue readyQ;

	Worker *master;

	Worker *current_thread;
	threadid_t nextId;

	uint64_t num_idle;
	uint64_t num_active_tasks;
	uint64_t max_allowed_active_workers;

	uint64_t num_workers;

	bool in_no_switch_region_;

	Worker *nextCoroutine(bool isBlocking=true) {
		scheduler_context_switches++;
		if (in_no_switch_region_) {
			std::cerr << "Trying to context switch in no-switch region" << std::endl;
			exit(-1);
		}

		do {
			Worker *result;
			scheduler_count++;

			result = readyQ.dequeue();
			if (result != NULL) {
				return result;
			}
		} while (isBlocking);
		return NULL;
	}

public:
	TaskScheduler();
	void init(Worker *master);

	void set_no_switch_region(bool val) {
		in_no_switch_region_ = val;
	}
	bool in_no_switch_region() {
		return in_no_switch_region_;
	}

	Worker *get_current_thread() {
		return current_thread;
	}

	int64_t active_worker_count() {
		return this->num_active_tasks;
	}

	void shutdown_readyQ() {
		uint64_t count = 0;
		while (readyQ.length() > 0) {
			Worker *w = readyQ.dequeue();
			count++;
		}
		if (count > 0) {
			std::cerr << "Workers were found on readyQ at termination: " << count << std::endl;
		} else {
			std::cerr << "No Workers were found on readyQ at termination" << std::endl;
		}
	}

	void allow_active_workers(int64_t n) {
		if (n == -1) {
			max_allowed_active_workers = num_workers;
		} else {
			max_allowed_active_workers = n+1;
		}
	}

	int64_t max_allowed_active() {
		return max_allowed_active_workers;
	}

	void assignTid(Worker *thr) {
		thr->id = nextId++;
	}

	void createWorkers(uint64_t num);
	Worker *maybeSpawnCoroutines();
	void onWorkerStart();

	uint64_t active_task_count() {
		return num_active_tasks;
	}

	void ready(Worker *thr) {
		readyQ.enqueue(thr);
	}

	void run();

	bool thread_maybe_yield();
	bool thread_yield();
	bool thread_yield_periodic();
	void thread_suspend();
	void thread_wake(Worker *next);
	void thread_yield_wake(Worker *next);
	void thread_suspend_wake(Worker *next);
	bool thread_idle(uint64_t total_idle);
	bool thread_idle();

	Worker *thread_wait(void **result);

	void thread_on_exit();
};

inline bool TaskScheduler::thread_yield() {
	if (current_thread == master) {
		std::cerr << "can't yield on a system Worker" << std::endl;
	}

	ready(current_thread);

	Worker *yieldedThr = current_thread;
	Worker *next = nextCoroutine();
	bool gotRescheduled = (next==yieldedThr);
	
	current_thread = next;

	thread_context_switch(yieldedThr, next, NULL);

	return gotRescheduled;
}

inline void TaskScheduler::thread_suspend() {
	Worker *yieldedThr = current_thread;
	yieldedThr->running = 0;
	yieldedThr->suspended = 1;
	Worker *next = nextCoroutine();
	current_thread = next;
	thread_context_switch(yieldedThr, next, NULL);
}

inline void TaskScheduler::thread_wake(Worker *next) {
	next->suspended = 0;
	ready(next);
}

inline void TaskScheduler::thread_yield_wake(Worker *next) {
	next->suspended = 0;

	Worker *yieldedThr = current_thread;
	ready(yieldedThr);

	current_thread = next;
	thread_context_switch(yieldedThr, next, NULL);
}

inline void TaskScheduler::thread_suspend_wake(Worker *next) {
	next->suspended = 0;

	Worker *yieldedThr = current_thread;
	yieldedThr->suspended = 1;

	current_thread = next;
	thread_context_switch(yieldedThr, next, NULL);
}

inline void TaskScheduler::thread_on_exit() {
	Worker *exitedThr = current_thread;
	current_thread = master;
	thread_context_switch(exitedThr, master, (void*)exitedThr);
}

extern TaskScheduler global_scheduler;

inline Worker *current_worker() {
	return global_scheduler.get_current_thread();
}

inline void yield() {
	global_scheduler.thread_yield();
}

inline void suspend() {
	global_scheduler.thread_suspend();
}

inline void wake(Worker *t) {
	global_scheduler.thread_wake(t);
}

inline void yield_wake(Worker *t) {
	global_scheduler.thread_yield_wake(t);
}

inline void suspend_wake(Worker *t) {
	global_scheduler.thread_suspend_wake(t);
}

//for WEBUI mainly
double nsec(const timeval& start, timeval& end); 
//


#endif //_TASK_SCHEDULER_H
