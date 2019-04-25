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

// print function
void print(const std::string &s) {
    bool isping = (s == std::string("ping"));
    for (int i = 0; i < LIMIT; i++) {
        if (isping) {
            s1.enter();
        }
        else {
            s2.enter();
        }
        std::cout << s << "\n";
        if (isping) {
            s2.leave();
        }
        else {
            s1.leave();
        }
        
    }
}

int main() {

    // creating thread instances
    std::thread thping(print, "ping"), thpong(print, "pong");

    thping.join();
    thpong.join();

    return 0;
}