#include "Accirr.h"


Worker *master_thread;

void AccirrInit(int *argc_p, char **argv_p[]) {
	master_thread = convert_to_master();

	global_scheduler.init(master_thread);
}

void createTask(thread_func f, void *arg) {
	Worker *t = worker_spawn(global_scheduler.get_current_thread(), &global_scheduler, f, arg);
	global_scheduler.ready(t);
}

int AccirrFinalize() {
	destroy_thread(master_thread);
	return 0;
}

void AccirrRun() {
	global_scheduler.run();
}

