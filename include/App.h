#ifndef THREADPOOL_APP_H
#define THREADPOOL_APP_H

#include "Config.h"
#include "FileProcessor.h"
#include "ThreadPool.h"

class App {
    Config conf;
public:

    App(Config config);

    void handle_results(std::vector<std::future<FileResult>> &results);

    void run();
};


#endif //THREADPOOL_APP_H