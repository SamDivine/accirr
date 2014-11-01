#include "Accirr.h"

int total_switch = 0;

void mytask(Worker *me, void *arg) {
	total_switch++;
}

int main(int argc, char** argv)
{
	AccirrInit(&argc, &argv);
	for (int i = 0; i < 100; i++) {
		createTask(mytask, NULL);
	}
	AccirrRun();
	AccirrFinalize();
	std::cerr << "total_switch: " << total_switch << std::endl;
	return 0;
}

