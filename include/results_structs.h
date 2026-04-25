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