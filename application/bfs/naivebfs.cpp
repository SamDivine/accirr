#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;

int* vertexIndex;
int* edgeData;
int* linkedList;

int source = 0;

int edgeNum = 20;
int vertexNum = 10;

int* searchedList;
int nowIdx = 0;
int endIdx = 0;

int* parent;

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

void bfs(int src) {
	searchedList = new int[vertexNum];
	parent = new int[vertexNum];
	for (int i = 0; i < vertexNum; i++) {
		searchedList[i] = -1;
		parent[i] = -1;
	}
	searchedList[nowIdx] = src;
	endIdx++;
	while (nowIdx != endIdx) {
		int mVertex = searchedList[nowIdx];
		nowIdx = (nowIdx+1 < vertexNum) ? nowIdx+1 : 0;
		int mVertexIndex = vertexIndex[mVertex];
		int nextVertexIndex = (mVertex+1 < vertexNum) ? vertexIndex[mVertex+1] : edgeNum*2;
		for (int i = mVertexIndex; i < nextVertexIndex; i++) {
			int tmpVertex = linkedList[i];
			if (tmpVertex == src) {
				break;
			}
			if (parent[tmpVertex] == -1) {
				parent[tmpVertex] = mVertex;
				searchedList[endIdx] = tmpVertex;
				endIdx = (endIdx+1 < vertexNum) ? endIdx+1 : 0;
			}
		}
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
	delete[] searchedList;
	delete[] parent;
}

int main(int argc, char** argv)
{
	struct timeval start, end;
	source = atoi(argv[2]);
	gettimeofday(&start, NULL);
	getGraph(argv[1]);
	gettimeofday(&end, NULL);
	double duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "get Graph takes " << duration << " s" << endl;
	gettimeofday(&start, NULL);
	bfs(source);
	gettimeofday(&end, NULL);
	duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "bfs " << duration << " s" << endl;
	cout << "bfs " << duration << " s" << endl;
	printPath(rand()%vertexNum);
	finalize();
	return 0;
}

