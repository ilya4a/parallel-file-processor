#include <iostream>

#include "include/App.h"
#include "include/Config.h"
#include "include/ThreadPool.h"

std::mutex cout_m;

int test_task(int id) {

   std::unique_lock<std::mutex> lock(cout_m);
   std::cout << "task " << id << " in thread: "
             << std::this_thread::get_id() << "\n";
   lock.unlock();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   return id*id;
}

void run_pool() {
    ThreadPool pool(4, 8);
    std::vector<std::future<int>> results;

    for (int i = 0; i < 20; ++i) {
        results.push_back(pool.add_task(test_task, i));
    }

    pool.wait_all();

    for (auto& i: results) {
        std::cout << i.get() << std::endl;
    }
}

int main() {
    Config config = Config::Builder()
    .set_root_path("../test_dir2 ../test_dir")
    .set_query("777777777777777777")
    .set_replacement("s...")
    .set_sensitive(false)
    .build();

    App app;
    app.run(config);

    return 0;
}

