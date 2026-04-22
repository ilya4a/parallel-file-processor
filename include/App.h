//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_APP_H
#define THREADPOOL_APP_H
#include <iostream>

#include "Config.h"
#include "FileCollector.h"
#include "../FileProcessor.h"


class App {
public:
    void run(Config conf) {
        std::vector<fs::path> files = FileCollector::collectRecursively(conf.root_path, conf.extensions);
        for (auto &i: files) std::cout << i << std::endl;

        FileProcessor fp(files[0], conf);

        for (auto& i: fp.get_file_result().search_result.matches) {
            std::cout << "line: " << i.line << " col: " << i.column << std::endl;
        }
    };
};


#endif //THREADPOOL_APP_H