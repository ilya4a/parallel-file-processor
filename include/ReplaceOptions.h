#ifndef THREADPOOL_REPLACEOPTIONS_H
#define THREADPOOL_REPLACEOPTIONS_H

#include <filesystem>
#include "utils.h"

namespace fs = std::filesystem;

struct ReplaceOptions {
    const std::string &replacement() const { return replacement_; }
    const fs::path &tmp_file_path() const { return tmp_file_path_; }
    std::string_view content() const { return content_; }
    const SearchResult &search_result() const { return search_result_; }
    size_t replacement_size() const { return replacement_size_; }

    class Builder {
        std::string replacement_;
        fs::path tmp_file_path_;
        std::string_view content_;
        SearchResult const &search_result_;
        size_t replacement_size_;

        bool is_build;

    public:
        Builder(std::string_view content, SearchResult const &search_result);

        Builder &set_replacement(std::string replacement);

        Builder &set_tmp_file_path(fs::path tmp_file_path);

        ReplaceOptions build();
    };

private:
    std::string replacement_;
    fs::path tmp_file_path_;
    std::string_view content_;
    SearchResult const &search_result_;
    size_t replacement_size_;

    ReplaceOptions(std::string &&replacement,
                   fs::path &&tmp_file_path,
                   std::string_view content,
                   SearchResult const &search_result,
                   size_t replacement_size);
};


#endif //THREADPOOL_REPLACEOPTIONS_H
