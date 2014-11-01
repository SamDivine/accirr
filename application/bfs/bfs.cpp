#include "Accirr.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;

int MAX_COROS;

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

int realCoros;

void getGraph(char* filename) {
	FILE *fp = fopen(filename, "r");
	fread(&vertexNum, sizeof(int), 1, fp);
	fread(&edgeNum, sizeof(int), 1, fp);
	vertexIndex = new int[vertexNum];
	for (int i = 0; i < vertexNum; i++) {
		vertexIndex[i] = 0;
	}
	int* index = new int[vertexNum];

	edgeData = new int[2*edgeNum];
	linkedList = new int[2*edgeNum];
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
	delete[] index;
}

void spreadTask(Worker *me, void *arg) {
	intptr_t idx = (intptr_t)arg;
	int pointsPerCoro = cQLen/realCoros;
	int remainder = cQLen%realCoros;
	int mIdx = idx*pointsPerCoro + (idx>=remainder ? remainder : idx);
	int nextIdx = mIdx + pointsPerCoro + (idx>=remainder ? 0 : 1);
	for (int i = mIdx; i < nextIdx; i++) {
		int mVertex = currentQ[i];
#ifdef DATA_PREFETCH
		__builtin_prefetch(&vertexIndex[mVertex], 0, 3);
		yield();
#endif
		int mVertexIndex = vertexIndex[mVertex];
		int nextVertexIndex = (mVertex+1 < vertexNum) ? vertexIndex[mVertex+1] : vertexNum;
#ifdef DATA_PREFETCH
		__builtin_prefetch(&linkedList[mVertexIndex], 0, 3);
		yield();
#endif
		for (int j = mVertexIndex; j < nextVertexIndex; j++) {
			int v = linkedList[j];
			if (v == source) {
				continue;
			}
#ifdef DATA_PREFETCH
			__builtin_prefetch(&parent[v], 1, 0);
			yield();
#endif
			if (parent[v] == -1) {
				parent[v] = mVertex;
				nextQ[nQLen++] = v;
			}
		}
	}
}

void bfs() {
	currentQ = new int[vertexNum];
	nextQ = new int[vertexNum];
	parent = new int[vertexNum];
	for (int i = 0; i < vertexNum; i++) {
		currentQ[i] = -1;
		nextQ[i] = -1;
		parent[i] = -1;
	}
	cQLen = 0;
	nQLen = 0;
	currentQ[cQLen++] = source;
	while (cQLen != 0) {
		realCoros = cQLen < MAX_COROS ? cQLen : MAX_COROS;
		for (intptr_t i = 0; i < realCoros; i++) {
			createTask(spreadTask, (void*)i);
		}
		AccirrRun();
		for (int i = 0; i < nQLen; i++) {
			currentQ[i] = nextQ[i];
		}
		cQLen = nQLen;
		nQLen = 0;
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
	delete[] vertexIndex;
	delete[] edgeData;
	delete[] linkedList;
	delete[] currentQ;
	delete[] nextQ;
	delete[] parent;
}

int main(int argc, char** argv)
{
	source = atoi(argv[2]);
	MAX_COROS = atoi(argv[3]);
	gettimeofday(&start, NULL);
	getGraph(argv[1]);
	AccirrInit(&argc, &argv);
	gettimeofday(&end, NULL);
	double duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "get Graph takes " << duration << " s" << endl;
	gettimeofday(&start, NULL);
	bfs();
	gettimeofday(&end, NULL);
	duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "bfs " << duration << " s" << endl;
	cout << "bfs " << duration << " s" << endl;
	AccirrFinalize();
	//srand((unsigned)time(NULL));
	printPath(rand()%vertexNum);
	finalize();
	return 0;
}

