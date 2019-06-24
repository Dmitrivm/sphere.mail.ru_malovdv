//
// Created by Dmitry Malov on 2019-06-24.
//

#pragma once

#include <mutex>
#include <deque>

// Thread safe implementation of a Queue using a std::Deque
template <typename T>
class SafeDeque {
private:
    std::deque<T> m_queue;
    std::mutex m_mutex;
public:
    SafeDeque() = default;

    ~SafeDeque() = default;

    bool empty() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    int size() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    void push_back(T t) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push_back(t);
    }

    void pop_front() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.pop_front();
    }

    T& front() {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.front();
    }
};