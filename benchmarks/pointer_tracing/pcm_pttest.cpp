#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <stdint.h>
#include <sched.h>

#include "cpucounters.h"
#include "utils.h"

int THREAD_NUM = 2;
int TOTAL_LISTS = (1<<11);
int LIST_LEN = (1<<15);
int REPEAT_TIMES = 1;

#ifndef LOCAL_NUM
#define LOCAL_NUM 14
#endif

int64_t total_accum = 0;
int64_t tra_times = 0;

struct timeval start, end;

class List {
public:
	int data[LOCAL_NUM];
	List* next;
	List() : next(NULL) {
        for (int i = 0; i < LOCAL_NUM; i++) {
            data[i] = 0;
        }
    }
};

List** head;
List** allList;

int* listsLen;
int* listNumber;

int tofillLists = TOTAL_LISTS;

void insertToListI(int i, List* l) {
	if (head[i] == NULL) {
		head[i] = l;
	} else {
		allList[i]->next = l;
	}
	allList[i] = l;
	//allList[i]->next = head[i];
	allList[i]->next = NULL;
}

void buildList() {
    int value = 0;
	for(int i = 0; i < TOTAL_LISTS; i++) {
		head[i] = NULL;
		listsLen[i] = 0;
		listNumber[i] = i;
	}
	// build list using malloc
	int idx;
	while (tofillLists > 0) {
		idx = rand()%tofillLists;
#ifdef USING_MALLOC
		List* tmp = (List*)malloc(sizeof(List));
#else
		List* tmp = new List();
#endif
		for (int i = 0; i < LOCAL_NUM; i++) {
			tmp->data[i] = value++;
		}
		insertToListI(listNumber[idx], tmp);
		listsLen[listNumber[idx]]++;
		if (listsLen[listNumber[idx]] == LIST_LEN) {
			listNumber[idx] = listNumber[tofillLists-1];
			tofillLists--;
		}
	}
	//
	for (int i = 0; i < TOTAL_LISTS; i++) {
		allList[i] = head[i];
	}
}

void tracingTask(int idx) {
	// TODO: arg parse
	int listsPerCoro = TOTAL_LISTS/THREAD_NUM;
	int remainder = TOTAL_LISTS%THREAD_NUM;
	int mListIdx = idx*listsPerCoro + (idx>=remainder ? remainder : idx);
	int nextListIdx = mListIdx + listsPerCoro + (idx>=remainder ? 0 : 1);
	List* localList;
	//
	int64_t accum = 0;
	int64_t times = 0;
	// TODO: tracing
	for (int i = 0; i < REPEAT_TIMES; i++) {
		for (int j = mListIdx; j < nextListIdx; j++) {
			localList = head[j];
			while (localList != NULL) {
				for (int k = 0; k < LOCAL_NUM; k++) {
					accum += localList->data[k];
				}
				times++;
				localList = localList->next;
			} 
		}
	}
	//
	total_accum += accum;
	tra_times += times;
}

void destroyList() {
	// free list
#ifdef USING_MALLOC
	free(listsLen);
	free(listNumber);
#else
	delete[] listsLen;
	delete[] listNumber;
#endif
	List* listNode;
	List* tmp;
	for (int i = 0; i < TOTAL_LISTS; i++) {
		listNode = head[i];
		do {
			tmp = listNode;
			listNode = listNode->next;
#ifdef USING_MALLOC
			free(tmp);
#else
			delete tmp;
#endif
		} while (listNode != NULL);
	}
#ifdef USING_MALLOC
	free(head);
	free(allList);
#else 
	delete[] head;
	delete[] allList;
#endif
}

int main(int argc, char** argv)
{
	set_signal_handlers();
    switch(argc) {
    case 5:
        LIST_LEN = (1<<atoi(argv[4]));
    case 4:
        TOTAL_LISTS = atoi(argv[3]);
	case 3:
		REPEAT_TIMES = atoi(argv[2]);
	case 2:
		THREAD_NUM = atoi(argv[1]);
        break;
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
	PCM *m = PCM::getInstance();
	PCM::CustomCoreEventDescription mEvents[4];
	uint64_t mCounts[4];
	mEvents[0].event_number = DTLB_LOAD_MISS_CAUSE_WALK_EVTNR;
	mEvents[0].umask_value = DTLB_LOAD_MISS_CAUSE_WALK_UMASK;
	mEvents[1].event_number = UOPS_ISSUED_ANY_EVTNR;
	mEvents[1].umask_value = UOPS_ISSUED_ANY_UMASK;
	mEvents[2].event_number = MEM_LOAD_UOPS_RETIRED_L2_HIT_EVTNR;
	mEvents[2].umask_value = MEM_LOAD_UOPS_RETIRED_L2_HIT_UMASK;
	mEvents[3].event_number = MEM_LOAD_UOPS_RETIRED_L2_MISS_EVTNR;
	mEvents[3].umask_value = MEM_LOAD_UOPS_RETIRED_L2_MISS_UMASK;
	if (m->good()) {
		m->resetPMU();
		m->program(PCM::CUSTOM_CORE_EVENTS, &mEvents);
	} else {
		std::cerr << "can't access PMU" << std::endl;
		return -1;
	}
#ifdef USING_MALLOC
    head = (List**)malloc(TOTAL_LISTS*sizeof(List*));
    allList = (List**)malloc(TOTAL_LISTS*sizeof(List*));
	listsLen = (int*)malloc(TOTAL_LISTS*sizeof(int));
	listNumber = (int*)malloc(TOTAL_LISTS*sizeof(int));
#else
    head = new List*[TOTAL_LISTS];
    allList = new List*[TOTAL_LISTS];
	listsLen = new int[TOTAL_LISTS];
	listNumber = new int[TOTAL_LISTS];
#endif
	gettimeofday(&start, NULL);
	buildList();
	gettimeofday(&end, NULL);
	double duration = (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)/1000000.0;
	std::cerr << "build duration = " << duration << std::endl;
	//getchar();
	gettimeofday(&start, NULL);
	SystemCounterState sstate1 = getSystemCounterState();
#pragma omp parallel for
	for (int i = 0; i < THREAD_NUM; i++) {
		cpu_set_t mask;
		CPU_ZERO(&mask);
		CPU_SET(processorid, &mask);
		if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
			std::cerr << "could not set CPU affinity in thread " << omp_get_thread_num() << std::endl;
		}
		tracingTask(i);
	}
	SystemCounterState sstate2 = getSystemCounterState();
	gettimeofday(&end, NULL);
	duration = (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)/1000000.0;
	for (int i = 0; i < 4; i++) {
		mCounts[i] = getNumberOfCustomEvents(i, sstate1, sstate2);
	}
	double tlbLoadMissPerSec = mCounts[0]/duration;
	double uopsAny = mCounts[1];
	double l2Hit = mCounts[2];
	double l2Miss = mCounts[3];
	double l2Load = l2Hit+l2Miss;
	double l2HitRatio = l2Hit/l2Load;
	std::cout << "traverse duration " << duration << " s tlbloadmiss/s " << tlbLoadMissPerSec << " uopsAny " << uopsAny << " l2hit,load,hitratio " << l2Hit << ", " << l2Load << ", " << l2HitRatio << std::endl;
	std::cerr << "traverse duration " << duration << " s accum " << total_accum << " traverse " << tra_times <<  " tlbloadmiss/s " << tlbLoadMissPerSec << " uopsAny " << uopsAny << " l2hit,load,hitratio " << l2Hit << ", " << l2Load << ", " << l2HitRatio << std::endl;

	destroyList();

	return 0;
}


