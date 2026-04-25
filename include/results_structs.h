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

    const std::string& replacement() const { return replacement_; }
    const fs::path& tmp_file_path() const { return tmp_file_path_; }
    std::string_view content() const { return content_; }
    const SearchResult& search_result() const { return search_result_; }
    size_t replacement_size() const { return replacement_size_; }

    class Builder {
        std::string replacement_;
        fs::path tmp_file_path_;
        std::string_view content_;
        SearchResult const& search_result_;
        size_t replacement_size_;

        bool is_build;

    public:
        Builder(std::string_view content, SearchResult const& search_result) :
        content_(content), search_result_(search_result), replacement_size_{0}
        {}

        Builder& set_replacement(std::string replacement) {
            replacement_ = std::move(replacement);
            replacement_size_ = replacement_.size();
            return *this;
        }

        Builder& set_tmp_file_path(fs::path tmp_file_path) {
            tmp_file_path_ = std::move(tmp_file_path);
            return *this;
        }

        ReplaceOptions build() {
            if (tmp_file_path_.empty()) throw std::runtime_error("ReplaceOptions: Builder: tmp_file_path is empty");
            if (is_build) throw std::runtime_error("ReplaceOptions: repeated call of build()");

            ReplaceOptions replace_options ( std::move(replacement_),
                std::move(tmp_file_path_),
                content_,
                search_result_,
                replacement_size_);

            is_build = true;
            return replace_options;
        }
    };
    

private:
    std::string replacement_;
    fs::path tmp_file_path_;
    std::string_view content_;
    SearchResult const& search_result_;
    size_t replacement_size_;

    ReplaceOptions(std::string&& replacement,
    fs::path&& tmp_file_path,
    std::string_view content,
    SearchResult const& search_result,
    size_t replacement_size)

    : replacement_(std::move(replacement)),
    tmp_file_path_(std::move(tmp_file_path)),
    content_(content),
    search_result_(search_result),
    replacement_size_(replacement_size){}
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