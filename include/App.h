//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_APP_H
#define THREADPOOL_APP_H
#include <iostream>

#include "Config.h"
#include "FileCollector.h"


class App {
public:
    void run(Config cnf) {
        std::vector<fs::path> files = FileCollector::collectRecursively(cnf.root_path, cnf.extensions);
        for (auto &i: files) std::cout << i << std::endl;
    };
};


#endif //THREADPOOL_APP_H