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

        for (auto const & path: files) {
            FileProcessor file_processor(path, conf);

            try {
                FileResult file_result = file_processor.process_file();

                std::cout << "path: " << path << std::endl;
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
            }catch (std::exception &e ) {
                std::cerr << "exception " << e.what() << " in file " << path.string() << std::endl;
            }


            std::cout << std::endl;
        }
    };
};


#endif //THREADPOOL_APP_H