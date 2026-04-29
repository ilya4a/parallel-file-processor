#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>

class ThreadPool {

    std::vector<std::string> exceptions;

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex m;

    std::condition_variable worker_cv;
    std::condition_variable queue_full_cv;
    std::atomic<bool> stop;

    std::atomic<size_t> active_tasks{0};
    std::condition_variable wait_finished_cv;
    size_t max_queue_size;

    void thread_task();


public:
    ThreadPool(size_t num_threads, size_t max_queue_size = std::numeric_limits<size_t>::max());

    void wait_all();

    template<typename F, typename ...Args>
    std::future<typename std::invoke_result_t<F, Args...> > add_task(F&& f, Args&&... args);

    ~ThreadPool();
};


template<typename F, typename ...Args>
std::future<typename std::invoke_result_t<F, Args...> > ThreadPool::add_task(F&& f, Args&&... args) {

    auto p_task_ptr = std::make_shared< std::packaged_task<typename std::invoke_result_t<F, Args...> ()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );


    std::unique_lock<std::mutex> lock(m);

    bool is_out_of_queue = false;
    if ( tasks.size() >= max_queue_size ) is_out_of_queue = true;

    queue_full_cv.wait(lock, [this](){return tasks.size() < max_queue_size || stop;});

    if (stop)  throw std::runtime_error("ThreadPool: add_task on stopped pool");

    tasks.emplace([p_task_ptr](){(*p_task_ptr)();});

    lock.unlock();
    worker_cv.notify_one();

    return p_task_ptr->get_future();
}



#endif //THREADPOOL_THREADPOOL_H
