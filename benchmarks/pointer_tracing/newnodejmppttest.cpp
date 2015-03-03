#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sched.h>

int JMP_DISTANCE = 2;
int TOTAL_LISTS = (1<<11);
int LIST_LEN = (1<<15);
int REPEAT_TIMES = 1;

#ifndef LOCAL_NUM
#define LOCAL_NUM 14
#endif

#ifndef MODE
#define MODE 1
#endif

#ifndef LOCALITY
#define LOCALITY 3
#endif

int64_t total_accum = 0;
int64_t tra_times = 0;

struct timeval start, end;

class List {
public:
	int* data;
	List* next;
	List* jp;
	List() : next(NULL), jp(NULL) {
#ifdef USING_MALLOC
		data = (int*)malloc(LOCAL_NUM*sizeof(int));
#else
		data = new int[LOCAL_NUM];
#endif
        for (int i = 0; i < LOCAL_NUM; i++) {
            data[i] = 0;
        }
    }
	~List() {
#ifdef USING_MALLOC
		//free(data);
#else
		delete[] data;
#endif
	}
};

List** head;
List** allList;

int* listsLen;
int* listNumber;


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
	int tofillLists = TOTAL_LISTS;
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
		new(tmp)List();
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

	for (int i = 0; i < TOTAL_LISTS; i++) {
		allList[i] = head[i];
	}
}

void setDistance(int dis) {
	for (int i = 0; i < TOTAL_LISTS; i++) {
		List* toJmp = head[i];
		for (int j = 0; j < dis; j++) {
			if (toJmp == NULL) {
				break;
			}
			toJmp = toJmp->next;
		}
		List* toSet = head[i];
		while (toJmp != NULL) {
			toSet->jp = toJmp;
			toSet = toSet->next;
			toJmp = toJmp->next;
		}
	}
}

void tracingTask() {
	// TODO: arg parse
	int listsPerCoro = TOTAL_LISTS;
	int remainder = 0;
	int idx = 0;
	int mListIdx = idx*listsPerCoro + (idx>=remainder ? remainder : idx);
	int nextListIdx = mListIdx + listsPerCoro + (idx>=remainder ? 0 : 1);
	List* localList;
	//
	int64_t accum = 0;
	int64_t times = 0;
	// TODO: tracing
	for (int j = mListIdx; j < nextListIdx; j++) {
		localList = head[j];
		while (localList != NULL) {
			__builtin_prefetch(localList->jp, MODE, LOCALITY);
			for (int i = 0; i < REPEAT_TIMES; i++) {
				for (int k = 0; k < LOCAL_NUM; k++) {
					accum += localList->data[k];
				}
			}
			times++;
			if (localList->jp) {
				__builtin_prefetch(localList->jp->data, MODE, LOCALITY);
			}
			localList = localList->next;
		}
	}
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
			free(tmp->data);
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

int bindProc(int bindid) {
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(bindid, &mask);
	if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
		std::cerr << "could not set CPU affinity in main thread " << std::endl;
		return -1;
	}
	return 0;
}

int main(int argc, char** argv)
{
    switch(argc) {
    case 5:
        LIST_LEN = (1<<atoi(argv[4]));
    case 4:
        TOTAL_LISTS = (1<<atoi(argv[3]));
	case 3:
		REPEAT_TIMES = atoi(argv[2]);
	case 2:
		JMP_DISTANCE = atoi(argv[1]);
        break;
    default:
        break;
    }
	int syscpu = sysconf(_SC_NPROCESSORS_CONF);
	bindProc(0);

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
	for (int i = 1; i <= JMP_DISTANCE; i++) {
		std::cerr << "jmp distance is " << i << std::endl;
		setDistance(i);
		std::cerr << "distance set finished" << std::endl;
		gettimeofday(&start, NULL);
		tracingTask();
		gettimeofday(&end, NULL);
		duration = (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)/1000000.0;
		std::cout << "traverse " << i << " duration " << duration << " s accum " << total_accum << " traverse " << tra_times << std::endl;
		std::cerr << "traverse " << i << " duration " << duration << " s accum " << total_accum << " traverse " << tra_times << std::endl;
	}

	destroyList();

	return 0;
}

