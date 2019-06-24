// Malov D.V.
// HW6 - external parallel merge sort
//
// Parameters:
// MAX_THREADS - number of threads in pool
// CHUNK_SIZE - number of integer numbers to sort in one chunk
// INPUT_FILE - input file to sort

#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <algorithm>
#include "ThreadPool.h"
#include "SafeDeque.h"

using namespace std;

constexpr uint CHUNK_SIZE = 5;
constexpr uint MAX_THREADS = 10;
const std::string INPUT_FILE = "input.txt";
constexpr uint _DEBUG_MODE = 0;


typedef std::vector<int> int_vec_t;

struct ChunkReader {
    fstream file;
    ChunkReader(string fn) {
        file.open(fn);
    }

    // get next chunk from file
    vector<int> getChunk(uint chsize=CHUNK_SIZE)
    {
        vector<int> vec;
        if (file.good()) {
            string word;

            int cnt = 0;
            while (file >> word)
            {
                int num = stoi(word);
                vec.push_back(num);
                ++cnt;

                if (cnt == chsize)
                    break;
            }
        }
        return vec;
    }
};

// sort of chunk and save to unique file
static void localSort(vector<int> v, int fileid, SafeDeque<int>& idtable) {
    // sort vector
    ofstream outfile;
    outfile.open (string("sort") + std::to_string(fileid));
    std::sort(v.begin(), v.end());

    for (auto it=v.begin(); it != v.end(); ++it) {
        outfile << *it << " ";
    }
    outfile.close();

    // save fileid to file tables when ready
    idtable.push_back(fileid);
}

static void MergeTwoFiles(uint inid1, uint inid2, uint outid, SafeDeque<int>& idtable) {
    fstream fin1, fin2;
    ofstream fout;

    string fnin1 = "sort" + std::to_string(inid1);
    string fnin2 = "sort" + std::to_string(inid2);
    string fnout =  "sort" + std::to_string(outid);

    fin1.open(fnin1);
    fin2.open(fnin2);
    fout.open(fnout);

    int v1, v2;
    if (!(fin1 >> v1)) throw std::runtime_error("Unexpected. Empty file!");
    if (!(fin2 >> v2)) throw std::runtime_error("Unexpected. Empty file!");

    bool endof1 = false;
    bool endof2 = false;

    bool existlastitem = false;
    int lastitem = 0;

    while (!(endof1 && endof2)) {
        if (endof1)
        {
            if (fin2 >> v2) {
                if (existlastitem) {
                    if (lastitem <= v2) {
                        fout << lastitem << " ";
                        lastitem = v2;
                        continue;
                    }
                    else {
                        fout << v2 << " ";
                    }
                }
            } else
            {
                endof2 = true;
            }
            continue;
        }
        if (endof2) {
            if (fin1 >> v1) {
                if (existlastitem) {
                    if (lastitem <= v1) {
                        fout << lastitem << " ";
                        lastitem = v1;
                        continue;
                    }
                    else {
                        fout << v1 << " ";
                    }
                }

            } else {
                endof1 = true;
            }
            continue;
        }

        if (v1 < v2) {
            fout << v1 << " ";
            if (!(fin1 >> v1)) {
                endof1 = true;
                existlastitem = true;
                lastitem = v2;
            }
        }
        else {
            fout << v2 << " ";
            if (!(fin2 >> v2)) {
                endof2 = true;
                existlastitem = true;
                lastitem = v1;
            }
        }
    }

    if (existlastitem)
        fout << lastitem << " ";

    fin1.close();
    fin2.close();
    fout.close();

    idtable.push_back(outid);
}

int main()
{
    // reading input file and split it to chunks
    ChunkReader chr = ChunkReader(INPUT_FILE);
    vector<int> v;
    SafeDeque<int> idtable;
    uint idcounter = 0;

    // Create and initialize pool
    ThreadPool pool(MAX_THREADS);
    pool.init();

    while (!(v = chr.getChunk()).empty()) {
        if (_DEBUG_MODE > 0) {
            cout << "\n";
            for (auto it = v.begin(); it != v.end(); it++) {
                // debug output
                cout << *it << " ";
            }
        }

        auto future = pool.submit(localSort, v, idcounter, std::ref(idtable));
        ++idcounter;
    }
    pool.shutdown();

    if (_DEBUG_MODE > 0) cout << "\n\nidtable size " << idtable.size() << "\n";

    while (idtable.size() > 1) {
        uint id1 = idtable.front();
        idtable.pop_front();
        uint id2 = idtable.front();
        idtable.pop_front();

        cout << id1 << " + " << id2 << " " <<  " > " << idcounter << "\n";

        MergeTwoFiles(id1, id2, idcounter, std::ref(idtable));
        ++idcounter;
    }

    if (_DEBUG_MODE > 0) cout << "\n\nidtable size " << idtable.size() << " "; // << " Front element: " << idtable.front() << "\n";

    return 0;
}
