#include <iostream>
#include <list>
#include <iterator>
#include "numbers.dat"

// Data - массив
// Size - размер массива

const int MinInt = 1; // by task definition
const int MaxInt = 100000; // by task definition

int main(int argc, char* argv[])
{
	if (!(((argc % 2) == 0) | (argc < 2))) {

		std::list<int> simpleList;
	    int start = 0;
	    int stop = 0;
		int  simpleCnt = 0;
		bool isSimple = true;

	    // find max n number in pair list
	    int maxn = 0;
	    int currn = 0;
	    for (int i = 1; i < argc; i++) {
	    	currn = std::atoi(argv[i]);
	    	if (currn > maxn) {
	    		maxn = currn; 
	    	}
	    }

	    // find simple numbers from data array
	    for (int n=0; n < Size; n++) {
	    	// stop checking for simplicity if we've reached the max number from input
	    	if (Data[n] > maxn) {
	    		break;
	    	}
			if (Data[n] == 1) {
				continue;
			}
	    	for (int m=2; m < Data[n]; m++) {
	    		if (Data[n] == 2) {
	    			isSimple = true;
	    			break;
	    		}
	    		if (m*m > Data[n]) {
	    			isSimple = true;
	    			break;
	    		}
	    		if ((Data[n] % m) == 0) {
	    			isSimple = false;
	    			break;
	    		} 
	    		else {
	    			isSimple = true;
	    		}
	    	}
	    	if (isSimple)
	    	{
	    		simpleList.push_back(Data[n]);
	    	}
	    }

	    // checking ranges from input and count simple numbers
	    for (int i = 1; i < argc; i+=2) {
	        start = std::atoi(argv[i]);
	        stop = std::atoi(argv[i+1]);
	        if ((start >= MaxInt) | ((stop >= MaxInt))) {
	        	std::cout << -1 << std::endl;
	    		continue;
	        }
	    	if (stop < start)
	    	{
	    		std::cout << 0 << std::endl;
	    		continue;
	    	}

			std::list<int>::iterator it;
			simpleCnt = 0;
			int q_element;
			for (it = simpleList.begin(); it != simpleList.end(); ++it)
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
		return 0;
	}
	else
	{
		std::cout << -1 << std::endl;
		return -1;
	}
}