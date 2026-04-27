#ifndef THREADPOOL_APP_H
#define THREADPOOL_APP_H
#include <iostream>

#include "Config.h"
#include "FileCollector.h"
#include "FileProcessor.h"
#include "ThreadPool.h"


class App {
    Config conf;

    void parse_conf() {

    }

public:

    App(Config config) : conf(config){}

    void handle_results(std::vector<std::future<FileResult>> &results) {

        for (auto &i: results) {
            FileResult file_result = i.get();

            std::cout  << file_result.file_path << std::endl;
            //
            std::cout << "found: " << file_result.search_result.matches.size() << " matches" << std::endl;
            //
            //
            // for (auto& i: file_result.search_result.matches) {
            //     std::cout << "line: " << i.line << " col: " << i.column_bytes << std::endl;
            // }
            //
            // std::cout << "replaces:  " << file_result.replace_result.num_replacements << std::endl;
            // std::cout << "time:  " << file_result.processing_time_us << " ms" << std::endl;
            // std::cout << "total_size:  " << file_result.search_result.total_bytes << std::endl;
            std::cout << "total_words:  " << file_result.search_result.total_words << std::endl;
            // std::cout << "total_lines:  " << file_result.search_result.lines << std::endl;
            std::cout << std::endl;

        }

    }


    void run() {

        ThreadPool thread_pool{};

        std::vector<std::future<FileResult>> results;

        for (auto& i : conf.root_paths() ) {

            std::vector<fs::path> files = FileCollector::collectRecursively(i, conf.extensions());

            for (auto const & path: files) {
                FileProcessor file_processor(path, conf);

                results.push_back(thread_pool.add_task([this, path] () -> FileResult {
                    FileProcessor file_processor(path, conf);
                    return file_processor.process_file();
                }));
            }

        }

        thread_pool.wait_all();
        handle_results(results);
    };
};


#endif //THREADPOOL_APP_H