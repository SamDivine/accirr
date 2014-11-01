#include <iostream>
using namespace std;

int main()
{
	int start = 10000001;
	int end = start + 100;
	for (int i = start; i < end; i+=2) {
		bool isPrime = false;
		for (int j = 3; j < 9999; j++) {
			if (i%j == 0) {
				isPrime = true;
				break;
			}
		}
		if (isPrime) {
			cout << i << endl;
			break;
		}
	}
	return 0;
}

