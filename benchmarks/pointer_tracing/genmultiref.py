#!/usr/bin/python

import sys

if len(sys.argv) < 2:
    print 'need at least one integer for concurrency'
    sys.exit()
concurrency = sys.argv[1]
if len(sys.argv) >= 3:
    listNum = sys.argv[2]
else:
    listNum = '2048'

if int(listNum)%int(concurrency) != 0:
    print 'listNum%concurrency should be 0'
    sys.exit();

LOCAL_NUM=14

outputContent='''
#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sched.h>

int TOTAL_LISTS = '''
outputContent+=listNum
outputContent+=''';
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


void insertToListI(int i, List* l) {
	if (head[i] == NULL) {
		head[i] = l;
	} else {
		allList[i]->next = l;
	}
	allList[i] = l;
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
	for (int i = 0; i < TOTAL_LISTS; i++) {
		allList[i] = head[i];
	}
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
	int syscpu = sysconf(_SC_NPROCESSORS_CONF);
        int quarterCore = syscpu/4;
	int bindid = quarterCore;
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
        //bindProc(0);
	gettimeofday(&start, NULL);
	for (int i = 0; i < TOTAL_LISTS; i+='''
outputContent+=concurrency
outputContent+=''') {'''

for i in range(int(concurrency)):
    offset = bytes(i)
    outputContent+=('''
            allList[i+'''+offset+'''] = head[i+'''+offset+'''];''')

outputContent+='''
            while (1) {
                if (allList[i] == NULL) {
                    break;
                }'''

for i in range(int(concurrency)):
    offset = bytes(i)
    outputContent+=('''
                allList[i+'''+offset+'''] = allList[i+'''+offset+''']->next;''')
outputContent+='''
                if (allList[i] == NULL) {
                    break;
                }'''


"""
    for j in range(LOCAL_NUM):
        joffset = bytes(j)
        outputContent+=('''
                total_accum += allList[i+'''+offset+''']->data['''+joffset+'''];''')
                allList[i+'''+offset+'''] = allList[i+'''+offset+''']->next;
"""
for i in range(int(concurrency)):
    offset = bytes(i)
    outputContent+=('''
                for (int k = 0; k < LOCAL_NUM; k++) {
                    total_accum += allList[i+'''+offset+''']->data[k];
                }
                tra_times++;''')
outputContent+='''
            }
        }
	gettimeofday(&end, NULL);
	duration = (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec)/1000000.0;
	std::cout << "traverse duration " << duration << " s" << std::endl;
	std::cerr << "traverse duration " << duration << " s accum " << total_accum << " traverse " << tra_times << std::endl;

	destroyList();

	return 0;
}'''
fileHandle = open('multiref_source.cpp', 'w')

fileHandle.write(outputContent);

fileHandle.close();

