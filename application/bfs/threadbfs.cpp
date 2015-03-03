#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <sched.h>

using namespace std;

#ifndef MODE
#define MODE 1
#endif

#ifndef LOCALITY
#define LOCALITY 2
#endif

int* vertexIndex;
int* edgeData;
int* linkedList;

int source = 0;

int edgeNum;
int vertexNum;

int* currentQ;
int* nextQ;
int cQLen = 0;
int nQLen = 0;

int* parent;

struct timeval start, end;

int realCoros = 1;

void getGraph(char* filename) {
	FILE *fp = fopen(filename, "r");
	fread(&vertexNum, sizeof(int), 1, fp);
	fread(&edgeNum, sizeof(int), 1, fp);
#ifdef USING_MALLOC
	vertexIndex = (int*)malloc(vertexNum*sizeof(int));
	int* index = (int*)malloc(vertexNum*sizeof(int));

	edgeData = (int*)malloc(2*edgeNum*sizeof(int));
	linkedList = (int*)malloc(2*edgeNum*sizeof(int));
#else
	vertexIndex = new int[vertexNum];
	int* index = new int[vertexNum];

	edgeData = new int[2*edgeNum];
	linkedList = new int[2*edgeNum];
#endif
	for (int i = 0; i < vertexNum; i++) {
		vertexIndex[i] = 0;
	}
	int v1, v2;
	for (int i = 0; i < edgeNum; i++) {
		fread(&v1, sizeof(int), 1, fp);
		fread(&v2, sizeof(int), 1, fp);
		vertexIndex[v1]++;
		vertexIndex[v2]++;
		edgeData[2*i] = v1;
		edgeData[2*i+1] = v2;
	}

	for (int i = 1; i < vertexNum; i++) {
		vertexIndex[i] += vertexIndex[i-1];
	}

	for (int i = vertexNum-1; i > 0; i--) {
		vertexIndex[i] = vertexIndex[i-1];
		index[i] = vertexIndex[i];
	}
	vertexIndex[0] = 0;
	index[0] = 0;

	for (int i = 0; i < edgeNum; i++) {
		v1 = edgeData[2*i];
		v2 = edgeData[2*i+1];
		linkedList[index[v1]] = v2;
		linkedList[index[v2]] = v1;
		index[v1]++;
		index[v2]++;
	}
#ifdef GETINIT
	cerr << "vertexIndex:";
	for (int i = 0; i < vertexNum; i++) {
		cerr << vertexIndex[i] << " ";
	}
	cerr << endl;

	cerr << "linkedList:";
	for (int i = 0; i < edgeNum*2; i++) {
		cerr << linkedList[i] << " ";
	}
	cerr << endl;
#endif
	fclose(fp);
#ifdef USING_MALLOC
	free(index);
	free(edgeData);
#else
	delete[] index;
	delete[] edgeData;
#endif
}

void spreadTask() {
	int idx = 0;
	int pointsPerCoro = cQLen/realCoros;
	int remainder = cQLen%realCoros;
	int mIdx = idx*pointsPerCoro + (idx>=remainder ? remainder : idx);
	int nextIdx = mIdx + pointsPerCoro + (idx>=remainder ? 0 : 1);
	for (int i = mIdx; i < nextIdx; i++) {
		int mVertex = currentQ[i];
		int mVertexIndex = vertexIndex[mVertex];
		int nextVertexIndex = (mVertex+1 < vertexNum) ? vertexIndex[mVertex+1] : vertexNum;
		for (int j = mVertexIndex; j < nextVertexIndex; j++) {
			int v = linkedList[j];
			if (v == source) {
				continue;
			}
			if (parent[v] == -1) {
				parent[v] = mVertex;
				nextQ[nQLen++] = v;
			}
		}
	}
}

void bfs() {
#ifdef USING_MALLOC
	currentQ = (int*)malloc(vertexNum*sizeof(int));
	nextQ = (int*)malloc(vertexNum*sizeof(int));
	parent = (int*)malloc(vertexNum*sizeof(int));
#else
	currentQ = new int[vertexNum];
	nextQ = new int[vertexNum];
	parent = new int[vertexNum];
#endif
	for (int i = 0; i < vertexNum; i++) {
		currentQ[i] = -1;
		nextQ[i] = -1;
		parent[i] = -1;
	}
	cQLen = 0;
	nQLen = 0;
	currentQ[cQLen++] = source;
	int level = 0;
	while (cQLen != 0) {
		spreadTask();
		for (int i = 0; i < nQLen; i++) {
			currentQ[i] = nextQ[i];
		}
		cQLen = nQLen;
		nQLen = 0;
		level++;
	}
}

void printPath(int p) {
	int idx = p;
	if (parent[idx] == -1) {
		cerr << "no path" << endl;
	} else {
		cerr << idx;
		while (parent[idx] != -1) {
			idx = parent[idx];
			cerr << "<-" << idx;
		}
		cerr << endl;
	}
}

void finalize() {
#ifdef USING_MALLOC
	free(vertexIndex);
	free(linkedList);
	free(currentQ);
	free(nextQ);
	free(parent);
#else
	delete[] vertexIndex;
	delete[] linkedList;
	delete[] currentQ;
	delete[] nextQ;
	delete[] parent;
#endif
}

int bindProc(int bindid) {
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(bindid, &mask);
	if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
		cerr << "could not set CPU affinity to core " << bindid << endl;
		return -1;
	}
	return 0;
}

int main(int argc, char** argv)
{
	source = atoi(argv[2]);
	bindProc(0);
	gettimeofday(&start, NULL);
	getGraph(argv[1]);
	gettimeofday(&end, NULL);
	double duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "get Graph takes " << duration << " s" << endl;
	gettimeofday(&start, NULL);
	bfs();
	gettimeofday(&end, NULL);
	duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "bfs " << duration << " s" << endl;
	cout << "bfs " << duration << " s" << endl;
	//srand((unsigned)time(NULL));
	printPath(rand()%vertexNum);
	finalize();
	return 0;
}

