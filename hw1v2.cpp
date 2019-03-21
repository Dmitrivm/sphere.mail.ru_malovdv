#include <iostream>
#include <queue>
#include <iterator>
#include "numbers.dat"

// Data - массив
// Size - размер массива

int main(int argc, char* argv[])
{

	if (((argc % 2) == 0) | (argc < 2)) {
		std::cout << -1 << std::endl;
		exit(-1);
	}

	std::queue<int> simpleList;

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

    	if (Data[n] > maxn) {
    		break;
    	}
		if (Data[n] == 1) {
			continue;
		}
    	//std::cout << Data[n] << std::endl;
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
    			// std::cout <<  " false " << Data[n] << std::endl;
    		} 
    		else {
    			isSimple = true;
    		}
    	}
    	if (isSimple)
    	{
    		// std::cout << Data[n] << std::endl;
    		// simpleCnt++;
    		simpleList.push(Data[n]);
    		// std::cout << "Push" << Data[n] << std::endl;
    	}
    	// if (Data[n] > 100) break;
    }


	std::queue<int> tmp_q = simpleList;
	// while (!tmp_q.empty())
	// {		
	//     int q_element = tmp_q.front();
	//    	std::cout << q_element << std::endl;
	//     tmp_q.pop();
	// }

    for (int i = 1; i < argc; i+=2) {

        start = std::atoi(argv[i]);
        stop = std::atoi(argv[i+1]);

        // std::cout << start << " " << stop << std::endl;

        if ((start >= 100000) | ((stop >= 100000))) {
        	std::cout << -1 << std::endl;
    		continue;
        }

    	if (stop < start)
    	{
    		std::cout << 0 << std::endl;
    		continue;
    	}

		simpleCnt = 0;
		tmp_q = simpleList; //copy the original queue to the temporary queue
		while (!tmp_q.empty())
		{		
		    int q_element = tmp_q.front();
		   	// std::cout << q_element << " " << tmp_q.empty() << std::endl;
	    	if (q_element < start) {
		    	tmp_q.pop();
		    	// std::cout << "Pop" << std::endl;
	    		continue;
	    	}
	    	if (q_element > stop)
	    		break;	

		    // std::cout << q_element <<"\n";
		    tmp_q.pop();
		    simpleCnt++;
		}

	    std::cout << simpleCnt << std::endl;
	}
    return 0;
}