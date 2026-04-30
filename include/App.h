#ifndef THREADPOOL_APP_H
#define THREADPOOL_APP_H

#include <future>
#include <vector>

#include "Config.h"
#include "FileProcessor.h"

class App {
    Config conf;

    std::vector<Match> get_match_lines(size_t i, FileResult const &file_result);

    void print_detail_level3(FileResult &file_result);

    void handle_results(std::vector<FileResult> &results);

    void handle_future_results(std::vector<std::future<FileResult> > &future_results);

public:
    static constexpr size_t default_tab_width = 4;

    App(Config config);

    void run();

    void run_sequentially();
};


#endif //THREADPOOL_APP_H
