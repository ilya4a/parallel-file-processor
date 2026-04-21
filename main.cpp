#include <iostream>

#include "ThreadPool.h"

std::mutex cout_m;

int test_task(int id) {

   std::unique_lock<std::mutex> lock(cout_m);
   std::cout << "task " << id << " in thread: "
             << std::this_thread::get_id() << "\n";
   lock.unlock();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   return id*id;
}

int main() {

    ThreadPool pool(4);
    std::vector<std::future<int>> results;

    for (int i = 0; i < 10; ++i) {
        results.push_back(pool.add_task(test_task, i));
    }

    for (auto& i: results) {
        std::cout << i.get() << std::endl;
    }

    return 0;
}



