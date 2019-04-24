#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

// D.V. Malov
// Synchronization task - HW5

const size_t LIMIT = 500000;

// mutex wrapper class
class MutexFactory
{

    size_t cnt;
    std::mutex m;
    std::condition_variable cnd;

public:
    explicit MutexFactory(int initialValue = 1)
        : cnt(initialValue)
    {
    }

    void enter()
    {
        std::unique_lock<std::mutex> lock(m);
        cnd.wait(lock, 
            [this]()
            { 
                return cnt > 0;
            });
        --cnt;
    }

    void leave()
    {
        std::unique_lock<std::mutex> lock(m);
        ++cnt;
        cnd.notify_one();
    }
};

MutexFactory s1, s2(0);

// ping
void print1() {
    for (int i = 0; i < LIMIT; i++) {
        s1.enter();
        std::cout << "ping" << "\n";
        s2.leave();
    }
}

//pong
void print2() {
    for (int i = 0; i < LIMIT; i++) {
        s2.enter();
        std::cout << "pong" << "\n";
        s1.leave();
    }
}

int main() {

    // creating thread instances
    std::thread thping(print1), thpong(print2);

    thping.join();
    thpong.join();

    return 0;
}