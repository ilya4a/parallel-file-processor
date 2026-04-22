//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_RESULTS_STRUCTS_H
#define THREADPOOL_RESULTS_STRUCTS_H
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


struct Match {
    size_t line;
    size_t column;

    Match(size_t line, size_t column) : line(line), column(column){};
};

struct SearchOptions {
    std::string find;
    bool use_regex;
    bool case_sensitive;
    SearchOptions(std::string str, bool regex = false, bool sensitive = false) : find(std::move(str)),
        use_regex(regex),
        case_sensitive(sensitive){};
};

struct SearchResult {
    std::vector<Match> matches;
};

struct ReplaceOptions {
    SearchOptions search_options;
    std::string replacement;
};

struct ReplaceResult {

};

struct FileResult {
    SearchResult search_result;
    ReplaceResult replace_result;
};



#endif //THREADPOOL_RESULTS_STRUCTS_H