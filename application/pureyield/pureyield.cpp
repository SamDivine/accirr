#include "Accirr.h"

#include <sys/time.h>

#include "customcounters.h"

int total_switch = 0;
int total_accum = 0;

int64_t TOTAL_TASKS = 64;
int64_t TOTAL_SWITCH = (1<<28);

void mytask(Worker *me, void *arg) {
	int accumNum;
	while(total_switch <= TOTAL_SWITCH) {
		accumNum = rand();
		if (accumNum%3 == 0) {
			yield();
			total_switch++;
		}
		total_accum += accumNum;
	}
}

int main(int argc, char** argv)
{
	switch(argc) {
	case 3:
		TOTAL_SWITCH = (1<<atoi(argv[2]));
	case 2:
		TOTAL_TASKS = atoi(argv[1]);
	default:
		break;
	}
	int syscpu = sysconf(_SC_NPROCESSORS_CONF);
	int processorid = 0;
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(processorid, &mask);
	if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
		std::cerr << "could not set CPU affinity in main thread " << std::endl;
	}
	if (customPcmInit() < 0) {
		return -1;
	}
	AccirrInit(&argc, &argv);
	for (int i = 0; i < TOTAL_TASKS; i++) {
		createTask(mytask, NULL);
	}
	CoreCounterState beforeState = getCoreCounterState(processorid);
	AccirrRun();
	CoreCounterState afterState = getCoreCounterState(processorid);
	AccirrFinalize();
	std::cerr << "total_switch: " << total_switch << " total_accum: " << total_accum << std::endl;
	customPcmPrint(beforeState, afterState, 0);
	return 0;
}

