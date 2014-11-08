#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "common.h"
#include "Worker.h"

class Scheduler {
	private:
		void dummy() {};
	public:
		virtual Worker *get_current_thread() = 0;
		virtual void assignTid(Worker *thr) = 0;

		virtual void workerNumInc() = 0;
		virtual void workerNumDec() = 0;
		virtual uint64_t getWorkerNum() = 0;
		
		virtual void ready(Worker *thr) = 0;
		virtual void run() = 0;

		virtual bool thread_yield() = 0;
		virtual void thread_suspend() = 0;
		virtual void thread_wake(Worker *next) = 0;
		virtual void thread_yield_wake(Worker *next) = 0;
		virtual void thread_suspend_wake(Worker *next) = 0;

		virtual Worker *thread_wait(void **result) = 0;
		virtual void thread_on_exit() = 0;
};

#endif //_SCHEDULER_H

