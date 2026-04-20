#include <iostream>

#include "ThreadPool.h"


int main() {

    ThreadPool pool(4);

    for (int i = 0; i < 10; ++i) {
        pool.add_task([i]() {
            std::cout << "task " << i << " in thread: "
                      << std::this_thread::get_id() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }

    return 0;
}
