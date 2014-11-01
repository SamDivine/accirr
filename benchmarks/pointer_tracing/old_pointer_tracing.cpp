#include <sys/time.h>
#include <iostream>
#include <stdlib.h>
#include <omp.h>

#ifndef LOCAL_NUM
#define LOCAL_NUM 12
#endif

int OMP_THREADS = 1;
int TOTAL_LISTS = 1;
int64_t TOTAL_DATA = (1<<28);

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

void insertToListI(int i, List* l) {
	if (head[i] == NULL) {
		head[i] = l;
	} else {
		allList[i]->next = l;
	}
	allList[i] = l;
	allList[i]->next = head[i];
}

void buildList() {
	for(int i = 0; i < TOTAL_LISTS; i++) {
		head[i] = NULL;
	}
    int value = 0;
	for (int i = 0; i < TOTAL_DATA; i++) {
		List* tmp = new List();
        for (int j = 0; j < LOCAL_NUM; j++) {
    		tmp->data[j] = value++;
        }
		insertToListI(rand()%TOTAL_LISTS, tmp);
	}
	for (int i = 0; i < TOTAL_LISTS; i++) {
		allList[i] = head[i];
	}
}

void tracingTask(int idx) {
	List* localList = allList[idx];
	List* localHead = head[idx];
	int64_t accum = 0;
    int64_t times = 0;
	if (localList != NULL) {
		do {
            for (int i = 0; i < LOCAL_NUM; i++) {
    			accum += localList->data[i];
            }
            times++;
		} while ((localList=localList->next) != localHead);
	}
    tra_times += times;
	total_accum += accum;
}

int main(int argc, char** argv)
{
    switch(argc) {
    case 4:
        TOTAL_DATA = (1<<atoi(argv[3]));
    case 3:
        TOTAL_LISTS = atoi(argv[2]);
    case 2:
        OMP_THREADS = atoi(argv[1]);
        break;
    default:
        break;
    }
    head = new List*[TOTAL_LISTS];
    allList = new List*[TOTAL_LISTS];
	gettimeofday(&start, NULL);
	buildList();
	gettimeofday(&end, NULL);
	long duration = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	//std::cerr << "build duration = " << duration << std::endl;
	gettimeofday(&start, NULL);
#pragma omp parallel for num_threads(OMP_THREADS)
	for (int i = 0; i < TOTAL_LISTS; i++) {
		tracingTask(i);
	}
	gettimeofday(&end, NULL);
	//std::cerr << "total_accum = " << total_accum << std::endl;
	duration = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	std::cout << "traverse duration " << duration << " us " << total_accum << " traverse " << tra_times << std::endl;
	std::cerr << "traverse duration " << duration << " us " << total_accum << " traverse " << tra_times << std::endl;

    delete[] head;
    delete[] allList;
	return 0;
}


