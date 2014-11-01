#include <sys/time.h>
#include <iostream>
#include <stdlib.h>
#include <omp.h>

#ifndef TOTAL_LISTS
#define TOTAL_LISTS 16
#endif

#ifndef TOTAL_DATA
#define TOTAL_DATA (1<<26)
#endif

#ifndef OMP_THREADS
#define OMP_THREADS 2
#endif

int64_t total_accum = 0;

struct timeval start, end;

class List {
public:
	int data;
	List* next;
	List() : data(0), next(NULL) {}
};

List* head[TOTAL_LISTS];
List* allList[TOTAL_LISTS];

void insertToListI(int i, List* l) {
	if (head[i] == NULL) {
		head[i] = l;
	} else {
		allList[i]->next = l;
	}
	allList[i] = l;
	allList[i]->next = head[i];
}

void buildList(int listNum, int listData) {
	for(int i = 0; i < listNum; i++) {
		head[i] = NULL;
	}
	for (int i = 0; i < listData; i++) {
		List* tmp = new List();
		tmp->data = i;
		insertToListI(rand()%listNum, tmp);
	}
	for (int i = 0; i < listNum; i++) {
		allList[i] = head[i];
	}
}

void tracingTask() {
	int traverseTimes = 0;
	while (traverseTimes < TOTAL_DATA) {
		for (int idx = 0; idx < TOTAL_LISTS; idx++) {
		
			if (allList[idx] != NULL) {
				total_accum += allList[idx]->data;
				allList[idx] = allList[idx]->next;
				traverseTimes++;
				if (traverseTimes >= TOTAL_DATA) {
					break;
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	gettimeofday(&start, NULL);
	buildList(TOTAL_LISTS, TOTAL_DATA);
	gettimeofday(&end, NULL);
	long duration = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	std::cerr << "build duration = " << duration << std::endl;
	gettimeofday(&start, NULL);
	tracingTask();
	gettimeofday(&end, NULL);
	std::cerr << "total_accum = " << total_accum << std::endl;
	duration = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	std::cerr << "traverse duration = " << duration << std::endl;
	return 0;
}


