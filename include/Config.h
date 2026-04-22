//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_CONFIG_H
#define THREADPOOL_CONFIG_H

#include <string>
#include <vector>
#include <filesystem>
#include <cstddef>
namespace fs = std::filesystem;

struct Config {
    fs::path root_path;
    Config(fs::path path) : root_path(path){};

};

#endif //THREADPOOL_CONFIG_H