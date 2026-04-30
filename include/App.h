#ifndef THREADPOOL_APP_H
#define THREADPOOL_APP_H

#include "Config.h"
#include "FileProcessor.h"

class App {
    Config conf;

    void print_detail_level3(FileResult &file_result);

    void handle_results(std::vector<FileResult> &results);

public:
    static constexpr size_t default_tab_width = 4;

    App(Config config);

    void run();

    void run_sequentially();
};


#endif //THREADPOOL_APP_H
