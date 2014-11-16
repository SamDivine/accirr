#include <Accirr.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>

#include "cpucounters.h"
#include "utils.h"

int CORO_NUM = 2;
int TOTAL_LISTS = (1<<11);
int LIST_LEN = (1<<15);
int REPEAT_TIMES = 1;

#ifndef PREFETCH_MODE
#define PREFETCH_MODE 0
#endif

#ifndef PREFETCH_LOCALITY
#define PREFETCH_LOCALITY 0
#endif

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

void tracingTask(Worker *me, void *arg) {
	// TODO: arg parse
	int listsPerCoro = TOTAL_LISTS/CORO_NUM;
	int remainder = TOTAL_LISTS%CORO_NUM;
	intptr_t idx = (intptr_t)arg;
	//std::cerr << "coro " << idx << " start" << std::endl;
	int mListIdx = idx*listsPerCoro + (idx>=remainder ? remainder : idx);
	int nextListIdx = mListIdx + listsPerCoro + (idx>=remainder ? 0 : 1);
	List* localList;
	//
	int64_t accum = 0;
	int64_t times = 0;
	// TODO: tracing
	for (int i = 0; i < REPEAT_TIMES; i++) {
		for (int j = mListIdx; j < nextListIdx; j++) {
			//std::cerr << "list " << j << " start" << std::endl;
			localList = head[j];
			while (localList != NULL) {
#ifdef DATA_PREFETCH
				__builtin_prefetch(localList, PREFETCH_MODE, PREFETCH_LOCALITY);
				yield();
#endif
				for (int k = 0; k < LOCAL_NUM; k++) {
					accum += localList->data[k];
				}
				times++;
				localList = localList->next;
			} 
			//std::cerr << "list " << j << " end" << std::endl;
		}
	}
	//
	total_accum += accum;
	tra_times += times;
	//std::cerr << "coro " << idx << " end" << std::endl;
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
		CORO_NUM = atoi(argv[1]);
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
	mEvents[1].event_number = DTLB_STORE_MISS_CAUSE_WALK_EVTNR;
	mEvents[1].umask_value = DTLB_STORE_MISS_CAUSE_WALK_UMASK;
	mEvents[2].event_number = L2_RQSTS_DEMAND_DATA_RD_HIT_EVTNR;
	mEvents[2].umask_value = L2_RQSTS_DEMAND_DATA_RD_HIT_UMASK;
	mEvents[3].event_number = L2_RQSTS_ALL_DEMAND_DATA_RD_EVTNR;
	mEvents[3].umask_value = L2_RQSTS_ALL_DEMAND_DATA_RD_UMASK;
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
	AccirrInit(&argc, &argv);
	gettimeofday(&start, NULL);
	buildList();
	gettimeofday(&end, NULL);
	double duration = (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)/1000000.0;
	std::cerr << "build duration = " << duration << std::endl;
	//getchar();
	for (intptr_t i = 0; i < CORO_NUM; i++) {
		createTask(tracingTask, (void*)i);
	}
	gettimeofday(&start, NULL);
	SystemCounterState sstate1 = getSystemCounterState();
	AccirrRun();
	AccirrFinalize();
	SystemCounterState sstate2 = getSystemCounterState();
	gettimeofday(&end, NULL);
	duration = (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)/1000000.0;
	for (int i = 0; i < 4; i++) {
		mCounts[i] = getNumberOfCustomEvents(i, sstate1, sstate2);
	}
	double tlbLoadMissPerSec = mCounts[0]/duration;
	double tlbStoreMissPerSec = mCounts[1]/duration;
	double l2Hit = mCounts[2];
	double l2Load = mCounts[3];
	double l2HitRatio = l2Hit/l2Load;
	std::cout << "traverse duration " << duration << "s tlbloadmiss/s " << tlbLoadMissPerSec << " tlbstoremiss/s " << tlbStoreMissPerSec << " l2hit,load,hitratio " << l2Hit << ", " << l2Load << ", " << l2HitRatio << std::endl;
	std::cerr << "traverse duration " << duration << " s accum " << total_accum << " traverse " << tra_times <<  " tlbloadmiss/s " << tlbLoadMissPerSec << " tlbstoremiss/s " << tlbStoreMissPerSec << " l2hit,load,hitratio " << l2Hit << ", " << l2Load << ", " << l2HitRatio << std::endl;

	destroyList();

	return 0;
}


