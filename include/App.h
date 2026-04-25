#ifndef THREADPOOL_APP_H
#define THREADPOOL_APP_H
#include <iostream>

#include "Config.h"
#include "FileCollector.h"
#include "FileProcessor.h"


class App {
public:
    void run(Config conf) {
        std::vector<fs::path> files = FileCollector::collectRecursively(conf.root_path(), conf.extensions());

        std::cout << "found: " << std::endl;

        for (auto const & path: files) {
            FileProcessor file_processor(path, conf);

            FileResult file_result = file_processor.process_file();

            std::cout << "path: " << path << std::endl;
            for (auto& i: file_result.search_result.matches) {
                std::cout << "line: " << i.line << " col: " << i.column_bytes << std::endl;
            }

            std::cout << "replaces:  " << file_result.replace_result.num_replacements << std::endl;

            std::cout << std::endl;
        }
    };
};


#endif //THREADPOOL_APP_H