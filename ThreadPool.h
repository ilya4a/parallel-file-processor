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

    // void add_task(std::function<void()> f);

    template<typename F, typename ...Args>
    std::future<typename std::result_of<F(Args...)>::type> add_task(F&& f, Args&&... args);

    ~ThreadPool();
};


template<typename F, typename ...Args>
std::future<typename std::result_of<F(Args...)>::type> ThreadPool::add_task(F&& f, Args&&... args) {

    auto p_task_ptr = std::make_shared< std::packaged_task<typename std::result_of<F(Args...)>::type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );


    std::unique_lock<std::mutex> lock(m);
    if (stop)  throw std::runtime_error("add_task on stopped pool");

    tasks.emplace([p_task_ptr](){(*p_task_ptr)();});

    lock.unlock();
    cv.notify_one();

    return p_task_ptr->get_future();
}



#endif //THREADPOOL_THREADPOOL_H
