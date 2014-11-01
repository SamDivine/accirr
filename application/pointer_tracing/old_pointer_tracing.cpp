#include <Accirr.h>
#include <sys/time.h>

int TOTAL_LISTS = 1;
int64_t TOTAL_DATA = (1<<28);

#ifndef PREFETCH_MODE
#define PREFETCH_MODE 0
#endif

#ifndef PREFETCH_LOCALITY
#define PREFETCH_LOCALITY 0
#endif

#ifndef LOCAL_NUM
#define LOCAL_NUM 12
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
	//std::cerr << "listData = " << listData << std::endl;
    int value = 0;
	for(int i = 0; i < TOTAL_LISTS; i++) {
		head[i] = NULL;
	}
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

void tracingTask(Worker *me, void *arg) {
	intptr_t idx = (intptr_t)arg;
	List* localList = allList[idx];
	List* localHead = head[idx];
	int64_t accum = 0;
	if (localList != NULL) {
		do {
#ifdef DATA_PREFETCH
			__builtin_prefetch(localList, PREFETCH_MODE, PREFETCH_LOCALITY);
			yield();
#endif
            for (int i = 0; i < LOCAL_NUM; i++) {
    			accum += localList->data[i];
            }
            tra_times++;
		} while ((localList=localList->next) != localHead);
	}
	total_accum += accum;
}

int main(int argc, char** argv)
{
    switch(argc) {
    case 3:
        TOTAL_DATA = (1<<atoi(argv[2]));
    case 2:
        TOTAL_LISTS = atoi(argv[1]);
        break;
    default:
        break;
    }
    head = new List*[TOTAL_LISTS];
    allList = new List*[TOTAL_LISTS];
	AccirrInit(&argc, &argv);
	gettimeofday(&start, NULL);
	buildList();
	gettimeofday(&end, NULL);
	long duration = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	//std::cerr << "build duration = " << duration << std::endl;
	for (intptr_t i = 0; i < TOTAL_LISTS; i++) {
		createTask(tracingTask, (void*)i);
	}
	gettimeofday(&start, NULL);
	AccirrRun();
	AccirrFinalize();
	gettimeofday(&end, NULL);
	duration = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	std::cout << "traverse duration " << duration << " us accum " << total_accum << " traverse " << tra_times << std::endl;
	std::cerr << "traverse duration " << duration << " us accum " << total_accum << " traverse " << tra_times << std::endl;

    delete[] head;
    delete[] allList;
	return 0;
}


