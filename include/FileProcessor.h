//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_FILEPROCECCOR_H
#define THREADPOOL_FILEPROCECCOR_H
#include <filesystem>
#include <fstream>
#include <utility>
#include "Config.h"
#include "results_structs.h"


class FileProcessor {
    Config config;
    fs::path file_path;
    FileResult file_result;
    std::string source;
    bool was_search;

    std::string readFileToString(const std::filesystem::path& path);
    std::string create_temp_file();

public:

    FileProcessor(fs::path path, Config conf) :config(std::move(conf)), file_path(std::move(path)), was_search(false) {}

    SearchResult search();

    FileResult replace();
};



#endif //THREADPOOL_FILEPROCECCOR_H