#include <iostream>
#include <list>
#include <vector>
#include <iterator>
#include "numbers.dat"

// compiler options
// g++ main.cpp -std=c++14 -o hw1final

// Data - массив
// Size - размер массива

const size_t MinInt = 1; // by task definition
const size_t MaxInt = 100000; // by task definition

// find max input number
int findMax(int argc, char* argv[])
{
	int maxn = 0;
	int currn = 0;
	for (int i = 1; i < argc; i++) {
		currn = std::atoi(argv[i]);
		if (currn > maxn) {
			maxn = currn; 
		}
	}
	return maxn;
}
	
// find simple numbers from data array
// return vector by value taking into account Return value optimization which convert it to a pointer
std::vector<int> findSimpleNumbers(const int data[], int maxn) {
	std::vector<int> sl;
	bool isSimple = true;
	for (int n=0; n < Size; n++) {
		// stop checking for simplicity if we've reached the max number from input
		if (data[n] > maxn) {
			break;
		}
		if (data[n] == 1) {
			continue;
		}
		for (int m=2; m < data[n]; m++) {
			if (data[n] == 2) {
				isSimple = true;
				break;
			}
			if (m*m > data[n]) {
				isSimple = true;
				break;
			}
			if ((data[n] % m) == 0) {
				isSimple = false;
				break;
			} 
			else {
				isSimple = true;
			}
		}
		if (isSimple)
		{
			sl.push_back(data[n]);
		}
	}
	return sl;
}

// checking ranges from input and count simple numbers
void calcNandPrint(const int data[], std::vector<int> simpleVector, int argc, char* argv[]) {
	int start = 0;
	int stop = 0;
	int  simpleCnt = 0;

	for (int i = 1; i < argc; i+=2) {
		start = std::atoi(argv[i]);
		stop = std::atoi(argv[i+1]);
		if ((start > MaxInt) | (stop > MaxInt) | (start < MinInt) | (stop < MinInt)) {
			std::cout << -1 << std::endl;
			continue;
		}
		if (stop < start)
		{
			std::cout << 0 << std::endl;
			continue;
		}

		simpleCnt = 0;
		int q_element;
		for (auto it = simpleVector.begin(); it != simpleVector.end(); ++it)
		{		
			q_element = *it;
			if (q_element < start) {
				continue;
			}
			if (q_element > stop)
				break;
			simpleCnt++;
		}
		std::cout << simpleCnt << std::endl;
	}
}

int main(int argc, char* argv[])
{
	if (!(((argc % 2) == 0) | (argc < 2))) {

		std::vector<int> simpleVector;
		// find max n number in pair list
		int maxn = findMax(argc, argv);
		// find simple numbers
		simpleVector = findSimpleNumbers(Data, maxn);
		// counte simple numbers in ranges 
		calcNandPrint(Data, simpleVector, argc, argv);

		return 0;
	}
	else
	{
		std::cout << -1 << std::endl;
		return -1;
	}
}