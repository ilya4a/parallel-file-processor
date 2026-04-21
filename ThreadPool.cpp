#include "ThreadPool.h"

#include <iostream>
#include <ostream>

void ThreadPool::thread_task() {
    while (true) {

        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m);
            cv.wait(lock, [this](){return !tasks.empty() || stop;});

            if (stop && tasks.empty()) return;

            task = std::move(tasks.front());
            tasks.pop();
        }
        task();

    }
}

ThreadPool::ThreadPool(int num_threads) {
    start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(&ThreadPool::thread_task, this);
    }
}


// void ThreadPool::add_task(std::function<void()> f) {
//     std::unique_lock<std::mutex> lock(m);
//     if (stop)  throw std::runtime_error("add_task on stopped pool");
//     tasks.emplace(std::move(f));
//     lock.unlock();
//     cv.notify_one();
// }

ThreadPool::~ThreadPool() {
    stop = true;
    cv.notify_all();
    for (auto& t : threads) t.join();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "total duration: " << duration << std::endl;
}



