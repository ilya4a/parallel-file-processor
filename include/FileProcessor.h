
#ifndef THREADPOOL_FILEPROCECCOR_H
#define THREADPOOL_FILEPROCECCOR_H

#include <filesystem>
#include <fstream>
#include <utility>

#include "Config.h"
#include "utils.h"

struct FileResult {
    SearchResult search_result;
    ReplaceResult replace_result;
    std::string error_message;
    std::string file_path;

    size_t processing_time_us = 0;

    FileResult() : replace_result(-1){}
};

class FileProcessor {
    Config config;
    fs::path file_path;
    std::string source;

    std::string readFileToString(const std::filesystem::path& path);
    std::string create_temp_file();

    SearchResult search();

    ReplaceResult replace(SearchResult const& search_result);

public:
    FileProcessor(fs::path path, Config conf) :config(std::move(conf)), file_path(std::move(path)){}
    FileResult process_file();

};


#endif //THREADPOOL_FILEPROCECCOR_H