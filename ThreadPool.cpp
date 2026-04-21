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

            ++active_tasks;
            task = std::move(tasks.front());
            tasks.pop();
        }

        try {
            task();
        } catch (const std::exception& e) {
            std::cerr << "task threw " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "task threw unknown exception" << std::endl;
        }

        --active_tasks;

        std::unique_lock<std::mutex> lock(m);
        if (active_tasks == 0 && tasks.empty()) {
            lock.unlock();
            wait_finished_cv.notify_all();
        }

    }
}

ThreadPool::ThreadPool(int num_threads) : start(std::chrono::steady_clock::now())   {
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(&ThreadPool::thread_task, this);
    }
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(m);
    wait_finished_cv.wait(lock, [this]() {
        return tasks.empty() && active_tasks == 0;
    });
}

ThreadPool::~ThreadPool() {
    stop = true;
    cv.notify_all();
    for (auto& t : threads) t.join();

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "total duration: " << duration << " ms\n";
}



