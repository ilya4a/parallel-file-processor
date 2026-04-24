//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_FILEPROCECCOR_H
#define THREADPOOL_FILEPROCECCOR_H
#include <filesystem>
#include <fstream>
#include <vector>
#include "include/Config.h"
#include "include/results_structs.h"
#include "include/TextSearcher.h"
#include <string_view>


namespace fs = std::filesystem;

class TextReplacer {
public:
    static ReplaceResult replace(std::string_view content,
                               std::string_view replacement, ReplaceOptions const& options){}
};

class FileProcessor {
    Config const& config;
    fs::path const& file_path;
    FileResult file_result;
    std::string source;
    bool was_search;

    std::string readFileToString(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary);

        if (!file) {
            throw std::runtime_error("FileProcessor: cannot open file: " + path.string());
        }

        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string content(size, '\0');

        if (!file.read(content.data(), size)) {
            throw std::runtime_error("FileProcessor: failed to read file: " + path.string());
        }
        return content;
    }

public:

    FileProcessor(fs::path const& path, Config const& conf) :config(conf), file_path(path), was_search(false) {}

    SearchResult search() {

        source = readFileToString(file_path);

        SearchOptions options(config.query, config.use_regex, config.case_sensitive);

        SearchResult search_result = TextSearcher::search(source, options);

        file_result.search_result = search_result;
        was_search = true;

        return search_result;
    }

    FileResult get_file_result() {
        if (!was_search){ search(); was_search = true;}

        return file_result;
    }


};


#endif //THREADPOOL_FILEPROCECCOR_H