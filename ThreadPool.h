//
// Created by ilya on 4/20/26.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H
#include <functional>
#include <future>
#include <queue>
#include <thread>


class ThreadPool {
    std::chrono::time_point<std::chrono::system_clock> start;

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex m;

    std::condition_variable cv;
    std::atomic<bool> stop;

    void thread_task();

public:
    ThreadPool(int num_threads);

    void add_task(std::function<void()> f);

    ~ThreadPool();
};



#endif //THREADPOOL_THREADPOOL_H
