#include "TaskScheduler.h"

TaskScheduler global_scheduler;

TaskScheduler::TaskScheduler()
	: readyQ()
	, master(NULL)
	, current_thread(NULL)
	, nextId(1)
	, num_idle(0)
	, num_active_tasks(0)
	, num_workers(0)
	, in_no_switch_region_(false) {}
void TaskScheduler::init(Worker *arg_master) {
	master = arg_master;
	readyQ.init(READYQ_PREFETCH_DISTANCE);
	current_thread = master;
}

void TaskScheduler::run() {
	while (thread_wait(NULL) != NULL) {}
}

Worker* TaskScheduler::thread_wait(void **result) {
	if (current_thread != master) {
		std::cerr << "only meant to be called by system worker" << std::endl;
		exit(-1);
	}

	Worker* next = nextCoroutine(false);
	if (next == NULL) {
		return NULL;
	} else {
		current_thread = next;
		Worker* died = (Worker*)thread_context_switch(master, next, NULL);
		if (result != NULL) {
			void *retval = (void*)died->next;
			*result = retval;
		}
		return died;
	}

}

double asecd() {
	timeval pNow;
	gettimeofday(&pNow, NULL);
	return pNow.tv_sec+pNow.tv_usec/1000000.0;
}

double asecd(timeval* pNow) {
	gettimeofday(pNow, NULL);
	return pNow->tv_sec+pNow->tv_usec/1000000.0;
}
