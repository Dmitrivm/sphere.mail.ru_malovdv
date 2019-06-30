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
const std::string INPUT_FILE = "input_binary.dat";
constexpr uint _DEBUG_MODE = 0;


typedef std::vector<int> int_vec_t;

struct ChunkReader {
    fstream file;
    ChunkReader(string fn) {
        file.open(fn, ios::binary | ios::in);
    }

    // get next chunk from file
    vector<int> getChunk(uint chsize=CHUNK_SIZE)
    {
        vector<int> vec;
        if (file.good()) {
            int cnt = 0;
            int num;
            while (file.peek() != EOF)
            {
                file.read(reinterpret_cast<char*>(&num), sizeof(int));
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
    outfile.open(string("sort") + std::to_string(fileid), ios::binary | ios::out);

    std::sort(v.begin(), v.end());

    for (auto it=v.begin(); it != v.end(); ++it) {
        // outfile << *it << " ";
        outfile.write(reinterpret_cast<const char*>(&(*it)), sizeof(int));
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
    string fnout = "sort" + std::to_string(outid);

    fin1.open(fnin1, ios::binary | ios::in);
    fin2.open(fnin2, ios::binary | ios::in);
    fout.open(fnout, ios::binary | ios::out);

    int v1, v2;
    if ((fin1.peek() == EOF) && (fin2.peek() == EOF)) throw std::runtime_error("Unexpected. Empty file!");
    fin1.read(reinterpret_cast<char*>(&v1), sizeof(int));
    fin2.read(reinterpret_cast<char*>(&v2), sizeof(int));

    bool endof1 = false;
    bool endof2 = false;

    bool existlastitem = false;
    int lastitem = 0;

    while (!(endof1 && endof2)) {
        if (endof1)
        {
            if (fin2.peek() != EOF) {
                fin2.read(reinterpret_cast<char*>(&v2), sizeof(int));
                if (existlastitem) {
                    if (lastitem <= v2) {
                        fout.write(reinterpret_cast<const char*>(&lastitem), sizeof(int));
                        lastitem = v2;
                        continue;
                    }
                    else {
                        fout.write(reinterpret_cast<const char*>(&v2), sizeof(int));
                    }
                }
            } else
            {
                endof2 = true;
            }
            continue;
        }
        if (endof2) {
            if (fin1.peek() != EOF) {
                fin1.read(reinterpret_cast<char*>(&v1), sizeof(int));
                if (existlastitem) {
                    if (lastitem <= v1) {
                        fout.write(reinterpret_cast<const char*>(&lastitem), sizeof(int));
                        lastitem = v1;
                        continue;
                    }
                    else {
                        fout.write(reinterpret_cast<const char*>(&v1), sizeof(int));
                    }
                }
            } else {
                endof1 = true;
            }
            continue;
        }

        if (v1 < v2) {
            fout.write(reinterpret_cast<const char*>(&v1), sizeof(int));
            if (fin1.peek() == EOF) {
                endof1 = true;
                existlastitem = true;
                lastitem = v2;
            } else {
                fin1.read(reinterpret_cast<char*>(&v1), sizeof(int));
            }
        }
        else {
            fout.write(reinterpret_cast<const char*>(&v2), sizeof(int));
            if (fin2.peek() == EOF) {
                endof2 = true;
                existlastitem = true;
                lastitem = v1;
            }
            else {
                fin2.read(reinterpret_cast<char*>(&v2), sizeof(int));
            }
        }
    }

    if (existlastitem)
        fout.write(reinterpret_cast<const char*>(&lastitem), sizeof(int));

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

    try {
        while (idtable.size() > 1) {
            uint id1 = idtable.front();
            idtable.pop_front();
            uint id2 = idtable.front();
            idtable.pop_front();

            cout << id1 << " + " << id2 << " " << " > " << idcounter << "\n";

            MergeTwoFiles(id1, id2, idcounter, std::ref(idtable));
            ++idcounter;
        }
    }
    catch (std::runtime_error& ex) {
        cout << "Merge error! " << ex.what() << "\n";
    }

    if (_DEBUG_MODE > 0) cout << "\n\nidtable size " << idtable.size() << " "; // << " Front element: " << idtable.front() << "\n";
    

    return 0;
}
