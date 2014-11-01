#include <Accirr.h>
#include <sys/time.h>
#include <stdlib.h>

int TOTAL_LISTS = 1;
int64_t TOTAL_DATA = (1<<28);
int64_t TOTAL_TRAVERSE_TIME = (1<<28);

#ifndef PREFETCH_MODE
#define PREFETCH_MODE 0
#endif

#ifndef PREFETCH_LOCALITY
#define PREFETCH_LOCALITY 3
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
    int next;
	List(){
        next = 0;
		for (int i = 0; i < LOCAL_NUM; i++) {
			data[i] = 0;
		}
	}
};

List *mSpace;
int *listHead;
int *mBitmap;

void buildList() {
	//std::cerr << "listData = " << listData << std::endl;
    int len = TOTAL_DATA;
    int idx;
    while (len > 1) {
        idx = rand()%len;
        int tmp = mBitmap[idx];
        mBitmap[idx] = mBitmap[len-1];
        mBitmap[len-1] = tmp;
        len--;
    }
	int value = 0;
    int listLen = TOTAL_DATA/TOTAL_LISTS;
    for (int i = 0; i < TOTAL_DATA; i++) {
        idx = mBitmap[i];
        if (i%listLen == 0) {
            listHead[i/listLen] = idx;
        }
        if (i%listLen == listLen-1 || i == TOTAL_DATA-1) {
            mSpace[idx].next = listHead[i/listLen];
        } else {
            mSpace[idx].next = mBitmap[i+1];
        }
        for (int j = 0; j < LOCAL_NUM; j++) {
            mSpace[idx].data[j] = value++;
        }
    }
}

void tracingTask(Worker *me, void *arg) {
	intptr_t idx = (intptr_t)arg;
	int64_t accum = 0;
	for (int i = 0; i < TOTAL_TRAVERSE_TIME/TOTAL_LISTS; i++) {
#ifdef DATA_PREFETCH
		__builtin_prefetch(mSpace+idx, PREFETCH_MODE, PREFETCH_LOCALITY);
		yield();
#endif
		for (int j = 0; j < LOCAL_NUM; j++) {
			accum += mSpace[idx].data[j];
		}
        idx = mSpace[idx].next;
        tra_times++;
	}
	total_accum += accum;
}

int main(int argc, char** argv)
{
    switch(argc) {
	case 4:
		TOTAL_TRAVERSE_TIME = (1<<atoi(argv[3]));
	case 3:
		TOTAL_DATA = (1<<atoi(argv[2]));
	case 2:
		TOTAL_LISTS = atoi(argv[1]);
		break;
	default:
		break;
	}

    srand((unsigned)time(NULL));
	mSpace = new List[TOTAL_DATA];
    listHead = new int[TOTAL_LISTS];
    mBitmap = new int[TOTAL_DATA];
    for (int i = 0; i < TOTAL_DATA; i++) {
        mBitmap[i] = i;
    }

	AccirrInit(&argc, &argv);
	gettimeofday(&start, NULL);
	buildList();
	gettimeofday(&end, NULL);
	long duration = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	//std::cerr << "build duration = " << duration << std::endl;
	for (intptr_t i = 0; i < TOTAL_LISTS; i++) {
		createTask(tracingTask, (void*)(listHead[i]));
	}
	gettimeofday(&start, NULL);
	AccirrRun();
	AccirrFinalize();
	gettimeofday(&end, NULL);
	//std::cerr << "total_accum = " << total_accum << std::endl;
	duration = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	std::cout << "traverse duration " << duration << " us accum " << total_accum << " traverse " << tra_times << std::endl;
	std::cerr << "traverse duration " << duration << " us accum " << total_accum << " traverse " << tra_times << std::endl;

	delete[] mSpace;
    delete[] listHead;
    delete[] mBitmap;

	return 0;
}


