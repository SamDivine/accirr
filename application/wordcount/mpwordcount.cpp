#include "Accirr.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>
#include <sched.h>

using namespace std;

const int BIG_PRIME = 100000001;

int CORO_NUM;
int PROC_NUM;
int processid = 0;

int64_t INITTEXTSIZE=(1<<24);
int64_t QUERYWORDS=(1<<23);

int64_t foundCount = 0;

struct timeval start, end;


#ifndef LOCAL_LEN
#define LOCAL_LEN 48
#endif


class hashNode {
public:
	char mWord[LOCAL_LEN];
	int64_t mCount;
	hashNode* next;
	hashNode() : mCount(1), next(NULL) {
		memset(mWord, 0, LOCAL_LEN);
	}
	hashNode(int count, char* mString) : mCount(count), next(NULL) {
		strcpy(mWord, mString);
	}
};

hashNode** mHashMap;
//ascii
int startIdx = 33;
int endIdx = 126;
int legalCount = endIdx-startIdx+1;

unsigned int BKDRHash(char *str) {
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
	        hash = hash * seed + (*str++);
	}

    return (hash & 0x7FFFFFFF);
}

void randomWord(char* mString) {
	int length = rand()%(LOCAL_LEN-1);
	for (int i = 0; i < length; i++) {
		mString[i] = startIdx+rand()%legalCount;
	}
	mString[length] = '\0';
}

void getWords() {
	char insertWord[LOCAL_LEN];
	int pos;
	hashNode* mNode;
	for (int i = 0; i < INITTEXTSIZE; i++) {
		randomWord(insertWord);
		pos = BKDRHash(insertWord)%BIG_PRIME;
		mNode = mHashMap[pos];
		if (mNode == NULL) {
			mHashMap[pos] = new hashNode(1, insertWord);
		} else {
			while (mNode) {
				if (strcmp(mNode->mWord, insertWord) == 0) {
					mNode->mCount++;
					break;
				}
				if (mNode->next == NULL) {
					mNode->next = new hashNode(1, insertWord);
					break;
				}
				mNode = mNode->next;
			}
		}
	}
}

void queryTask(Worker *me, void *arg) {
	intptr_t idx = (intptr_t)arg;
	int wordsPerCoro = QUERYWORDS/CORO_NUM;
	int remainder = QUERYWORDS%CORO_NUM;
	int mIdx = idx*wordsPerCoro + (idx>=remainder ? remainder : idx);
	int nextIdx = mIdx + wordsPerCoro + (idx>=remainder ? 0 : 1);
	int pos;
	hashNode* mNode;
	char queryWord[LOCAL_LEN];
	for (int i = mIdx; i < nextIdx; i++) {
		randomWord(queryWord);
		int idx = BKDRHash(queryWord)%BIG_PRIME;
#ifdef DATA_PREFETCH
		__builtin_prefetch(&mHashMap[idx], 0, 1);
		yield();
#endif
		hashNode* mNode = mHashMap[idx];
		while (mNode) {
#ifdef DATA_PREFETCH
			__builtin_prefetch(mNode, 0, 1);
			yield();
#endif
			if (strcmp(mNode->mWord, queryWord) == 0) {
				foundCount += mNode->mCount;
				break;
			}
			mNode = mNode->next;
		}
	}
}



void destroyAll() {
	for (int i = 0; i < BIG_PRIME; i++) {
		hashNode* tmp = mHashMap[i];
		hashNode* prev;
		while(tmp) {
			prev = tmp;
			tmp = tmp->next;
			delete prev;
		}
	}
	delete[] mHashMap;
}

int main(int argc, char** argv)
{

	CORO_NUM = atoi(argv[1]);
	PROC_NUM = atoi(argv[2]);
	int syscpu = sysconf(_SC_NPROCESSORS_CONF);
	while (processid < PROC_NUM-1) {
		if (fork() == 0) {
			break;
		} else {
			processid++;
		}
	}
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(processid%syscpu, &mask);
	if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
		std::cerr << "could not set CPU affinity in process " << processid << std::endl;
	}
	mHashMap = new hashNode*[BIG_PRIME];
	for (int i = 0; i < BIG_PRIME; i++) {
		mHashMap[i] = NULL;
	}
	AccirrInit(&argc, &argv);

	gettimeofday(&start, NULL);
	getWords();
	gettimeofday(&end, NULL);
	double duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "build wordlist takes " << duration << " s" << endl;
	for (intptr_t i = 0; i < CORO_NUM; i++) {
		createTask(queryTask, (void*)i);
	}
	gettimeofday(&start, NULL);
	AccirrRun();
	gettimeofday(&end, NULL);
	duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	ofstream fout;
	char logfile[32];
	sprintf(logfile, "mpwordcount_%d_%d_%d.log", CORO_NUM, PROC_NUM, processid);
	fout.open(logfile);
	cerr << "task duration " << duration << " s, foundCount " << foundCount << endl;
	fout << "task duration " << duration << " s, foundCount " << foundCount << endl;
	fout.close();

	AccirrFinalize();
	destroyAll();

	return 0;
}

