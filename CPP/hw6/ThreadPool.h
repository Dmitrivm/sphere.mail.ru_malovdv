//
// Created by Dmitry Malov Orig on 2019-06-24.
//

#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include <chrono>
#include <thread>

#include "SafeQueue.h"

//using result_t = decltype(f(args...));

class ThreadPool {
private:
    class ThreadWorker {
    private:
        int m_id;
        ThreadPool& m_pool;
    public:

//        ThreadWorker() : m_pool(m_pool) {
//            std::cout << " I've been created! " << "\n";
//        }

        ThreadWorker(ThreadPool& pool, const int id)
                : m_pool(pool), m_id(id) {
            // std::cout << id << " I've been created! " << "\n";
        }

        //void setId(const int id);

        void operator()() {
            std::function<void()> func;
            bool dequeued;
            while (!m_pool.m_shutdown) {
                {
                    std::unique_lock<std::mutex> lock(m_pool.m_conditional_mutex);
                    if (m_pool.m_queue.empty()) {
                        m_pool.m_conditional_lock.wait(lock);
                    }
                    dequeued = m_pool.m_queue.dequeue(func);
                }
                if (dequeued) {
                    func();
                }
            }
        }
    };

    bool m_shutdown;
    SafeQueue<std::function<void()>> m_queue;
    std::vector<std::thread> m_threads;
    std::mutex m_conditional_mutex;
    std::condition_variable m_conditional_lock;

    bool stopping = false;
    int num_threads;

public:
    explicit ThreadPool(const int n_threads)
            : num_threads(n_threads), m_shutdown(false) {
    }

    void setstop() {
        stopping = true;
    }
    bool getstop() {
        return stopping;
    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;

    ThreadPool & operator=(const ThreadPool &) = delete;
    ThreadPool & operator=(ThreadPool &&) = delete;

    // Inits thread pool
    void init() {
        m_threads.reserve(10);
        for (int i = 0; i < num_threads; ++i) {
            // m_threads[i] = ;
            m_threads.emplace_back(std::thread(ThreadWorker(*this, i)));
        }
    }

    // Waits until threads finish their current task and shutdowns the pool (fix by DVM! Did not take into account non-empty queue!)
    void shutdown() {

        setstop();
        while (!m_queue.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        m_shutdown = true;
        m_conditional_lock.notify_all();

        for (int i = 0; i < m_threads.size(); ++i) {
            if(m_threads[i].joinable()) {
                m_threads[i].join();
            }
        }
    }

    // Submit a function to be executed asynchronously by the pool
    template<typename F, typename...Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {

        // Create a function with bounded parameters ready to execute
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // Encapsulate it into a shared ptr in order to be able to copy construct / assign
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // Wrap packaged task into void function
        std::function<void()> wrapper_func = [task_ptr]() {
            (*task_ptr)();
        };

        if (!getstop()) {
            // Enqueue generic wrapper function
            m_queue.enqueue(wrapper_func);
            // Wake up one thread if its waiting
            m_conditional_lock.notify_one();
        }

        // Return future from promise
        return task_ptr->get_future();
    }
};