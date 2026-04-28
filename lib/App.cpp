#include "App.h"

#include "utils.h"

App::App(Config config) : conf(config){}

void App::handle_results(std::vector<std::future<FileResult>> &results)  {

    size_t matches_total = 0;
    size_t words_total = 0;
    size_t bytes_total = 0;
    size_t lines_total = 0;

    for (auto &i: results) {

        FileResult file_result = i.get();

        if (conf.detail_level() == 1) {

        }else if (conf.detail_level() == 2) {
            std::cout  << file_result.file_path << std::endl;
            std::cout << "total_words:  " << file_result.search_result.total_words << std::endl;
            std::cout << "found: " << file_result.search_result.matches.size() << " matches" << std::endl;
            std::cout << std::endl;
        }else if (conf.detail_level() == 3) {
            std::cout  << file_result.file_path << std::endl;
            std::cout << "found: " << file_result.search_result.matches.size() << " matches" << std::endl;

            std::cout << "total_words:  " << file_result.search_result.total_words << std::endl;
            std::cout << "time:  " << file_result.processing_time_us << " microseconds" << std::endl;
            std::cout << "total_size:  " << file_result.search_result.total_bytes << std::endl;
            std::cout << "total_lines:  " << file_result.search_result.lines << std::endl;

            for (auto& i: file_result.search_result.matches) {
                std::cout << "line: " << i.line << " col: " << i.column_bytes << std::endl;
            }
            std::cout << std::endl;
        }

        matches_total += file_result.search_result.matches.size();
        words_total += file_result.search_result.total_words;
        bytes_total += file_result.search_result.total_bytes;
        lines_total += file_result.search_result.lines;
    }

    if (conf.use_replacement()) {
        std::cout << "replaces total: " << matches_total << std::endl;
    }else {
        std::cout << "matches total: " << matches_total << std::endl;
    }
    std::cout << "words total: " << words_total << std::endl;
    std::cout << "bytes total: " << bytes_total << std::endl;
    std::cout << "lines total: " << lines_total << std::endl;
}


void App::run() {

    auto start = std::chrono::steady_clock::now();

    ThreadPool thread_pool{conf.thread_count()};
    std::vector<std::future<FileResult>> results;

    for (auto& i : conf.root_paths() ) {

        std::vector<fs::path> files = utils::collectFilesRecursively(i, conf.extensions());

        for (auto const & path: files) {
            FileProcessor file_processor(path, conf);

            results.push_back(thread_pool.add_task([this, path] () -> FileResult {
                FileProcessor file_processor(path, conf);
                return file_processor.process_file();
            }));
        }
    }

    thread_pool.wait_all();
    auto end = std::chrono::steady_clock::now();
    size_t processing_time_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    handle_results(results);

    std::cout << "TOTAL TIME: " << processing_time_us << " microseconds" << std::endl;
}




