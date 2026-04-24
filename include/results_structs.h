//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_RESULTS_STRUCTS_H
#define THREADPOOL_RESULTS_STRUCTS_H
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>

namespace fs = std::filesystem;


struct Match {
        size_t line;
        size_t column_bytes;
        size_t byte_pos;
        size_t length;

        Match(size_t line, size_t column_bytes,  size_t byte_pos) : line(line), column_bytes(column_bytes), byte_pos(byte_pos) {
            length = -1;
        };

    };

struct SearchOptions {
    std::string find;
    bool use_regex;
    bool case_sensitive;
    SearchOptions(std::string str, bool regex, bool sensitive) : find(std::move(str)),
        use_regex(regex),
        case_sensitive(sensitive){};
};

struct SearchResult {
    std::vector<Match> matches;
};

struct ReplaceOptions {
    // SearchOptions search_options;
    std::string replacement;
    fs::path tmp_file_path;
    std::string_view content;
    SearchResult const& search_result;
    size_t replacement_size;

    ReplaceOptions(std::string replacement,
    fs::path tmp_file_path,
    std::string_view content,
    SearchResult const& search_result)
    : replacement(std::move(replacement)),
    tmp_file_path(std::move(tmp_file_path)),
    content(content),
    search_result(search_result) {
        replacement_size = this->replacement.size();
        // std::cout << "replacement_size = replacement.size(): " << replacement_size << std::endl;
        // std::cout << "replacement " << replacement << std::endl;
    }
};

struct ReplaceResult {
    size_t num_replacements;
    ReplaceResult(size_t num) : num_replacements(num){};
};

struct FileResult {
    SearchResult search_result;
    ReplaceResult replace_result;
    std::string error_message;

    FileResult() : replace_result(-1){}
};



#endif //THREADPOOL_RESULTS_STRUCTS_H