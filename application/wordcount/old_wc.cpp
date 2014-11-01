#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;

const int BIG_PRIME = 10000001;

int TOTAL_WORDS = 0;

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

char** originWords;

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

void getWords(char* input) {
	FILE* fp = fopen(input, "r");
	fscanf(fp, "%d", &TOTAL_WORDS);
	originWords = new char*[TOTAL_WORDS];
	for (int i = 0; i < TOTAL_WORDS; i++) {
		originWords[i] = new char[LOCAL_LEN];
		fscanf(fp, "%s", originWords[i]);
	}
	fclose(fp);
}

void getCount() {
	int idx;
	hashNode* mNode;
	bool found = false;
	char tmp[LOCAL_LEN];
	for (int i = 0; i < TOTAL_WORDS; i++) {
		strcpy(tmp, originWords[i]);
		idx = BKDRHash(tmp)%BIG_PRIME;
		mNode = mHashMap[idx];
		if (mNode == NULL) {
			mHashMap[idx] = new hashNode(1, tmp);
		} else {
			while (mNode) {
				if (strcmp(mNode->mWord, tmp) == 0) {
					mNode->mCount++;
					break;
				}
				if (mNode->next == NULL) {
					mNode->next = new hashNode(1, tmp);
					break;
				}
				mNode = mNode->next;
			}
		}
	}
}

int findCount(char* mString) {
	int idx = BKDRHash(mString)%BIG_PRIME;
	hashNode* mNode = mHashMap[idx];
	int rst = 0;
	while (mNode) {
		if (strcmp(mNode->mWord, mString) == 0) {
			rst = mNode->mCount;
			break;
		}
		mNode = mNode->next;
	}
	return rst;
}

void randomWord(char* mString) {
	int length = rand()%(LOCAL_LEN-1);
	for (int i = 0; i < length; i++) {
		mString[i] = startIdx+rand()%legalCount;
	}
	mString[length] = '\0';
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
	for (int i = 0; i < TOTAL_WORDS; i++) {
		delete originWords[i];
	}
	delete[] originWords;
}

int main(int argc, char** argv)
{
	mHashMap = new hashNode*[BIG_PRIME];
	for (int i = 0; i < BIG_PRIME; i++) {
		mHashMap[i] = NULL;
	}
	
	gettimeofday(&start, NULL);
	getWords(argv[1]);
	gettimeofday(&end, NULL);
	double duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "build wordlist takes " << duration << " s" << endl;
	gettimeofday(&start, NULL);
	getCount();
	gettimeofday(&end, NULL);
	duration = (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
	cerr << "build maps takes " << duration << " s" << endl;
	

	char toFind[LOCAL_LEN];
	randomWord(toFind);
	int rst = findCount(toFind);

	cerr << "word: " << toFind << " count: " << rst << endl;
	destroyAll();

	return 0;
}

