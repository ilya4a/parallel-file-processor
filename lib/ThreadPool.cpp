#include "ThreadPool.h"

std::mutex excep_mut;

void ThreadPool::thread_task() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m);
            worker_cv.wait(lock, [this]() { return !tasks.empty() || stop; });

            if (stop && tasks.empty()) {
                return;
            }

            ++active_tasks;
            task = std::move(tasks.front());
            tasks.pop();
        }

        try {
            task();
        } catch (const std::exception &e) {
            std::unique_lock<std::mutex> lock(excep_mut);
            std::string s = e.what();
            exceptions.emplace_back("task threw " + s);
        } catch (...) {
            std::unique_lock<std::mutex> lock(excep_mut);
            exceptions.push_back("task threw unknown exception");
        }

        --active_tasks;

        std::unique_lock<std::mutex> lock(m);
        if (active_tasks == 0 && tasks.empty()) {
            lock.unlock();
            wait_finished_cv.notify_all();
        }

        queue_full_cv.notify_one();
    }
}

ThreadPool::ThreadPool(size_t num_threads, size_t max_queue_size) {
    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
    }

    this->max_queue_size = max_queue_size;
    for (size_t i = 0; i < num_threads; i++) {
        threads.emplace_back(&ThreadPool::thread_task, this);
    }
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(m);
    wait_finished_cv.wait(lock, [this]() { return tasks.empty() && active_tasks == 0; });
}

ThreadPool::~ThreadPool() {
    stop = true;

    {
        std::unique_lock<std::mutex> lock(m);
        worker_cv.notify_all();
        queue_full_cv.notify_all();
    }

    for (auto &t : threads) {
        t.join();
    }
}
