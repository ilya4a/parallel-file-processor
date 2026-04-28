
#ifndef THREADPOOL_UTILS_H
#define THREADPOOL_UTILS_H

#include <filesystem>
#include <vector>

struct ReplaceOptions;

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
    size_t total_words = 0;
    size_t total_bytes = 0;
    size_t lines = 0;

};

struct ReplaceResult {
    size_t num_replacements;
    std::string error_massage;
    size_t bytes;
    ReplaceResult(size_t num = 0) : num_replacements(num){};
};


namespace utils {


    std::vector<fs::path> collectFilesRecursively(const fs::path& root,  const std::vector<std::string>& allowed_extensions);

    ReplaceResult replace(ReplaceOptions& options);

    std::pair<size_t, size_t> find_line_and_column (std::vector<size_t> const& line_starts, size_t pos);
    SearchResult search(std::string_view content, const SearchOptions& options);

    void print_line_from_file(const fs::path& filePath, std::vector<Match>& matches, size_t match_len = 0);

}

#endif //THREADPOOL_UTILS_H