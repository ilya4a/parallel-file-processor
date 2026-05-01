#ifndef THREADPOOL_CONFIG_H
#define THREADPOOL_CONFIG_H

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct Config {
    const std::vector<fs::path> &root_paths() const { return root_paths_; }
    const std::vector<std::string> &extensions() const { return extensions_; }
    const std::string &query() const { return query_; }
    const std::string &replacement() const { return replacement_; }
    bool use_regex() const { return use_regex_; }
    bool case_sensitive() const { return case_sensitive_; }
    bool use_replacement() const { return use_replacement_; }
    bool file_info() const { return file_info_; }
    bool sequential() const { return sequential_; }

    size_t thread_count() const { return thread_count_; }
    size_t detail_level() const { return detail_level_; }

  private:
    std::vector<fs::path> root_paths_;
    std::vector<std::string> extensions_;
    std::string query_;
    std::string replacement_;
    bool use_regex_;
    bool case_sensitive_;
    bool use_replacement_;
    bool file_info_;
    bool sequential_;
    size_t thread_count_;
    size_t detail_level_;

  public:
    class Builder {
        std::vector<fs::path> root_paths_;
        std::vector<std::string> extensions_;
        std::string query_;
        std::string replacement_;
        bool use_regex_;
        bool case_sensitive_;
        bool use_replacement_;
        bool file_info_;
        bool sequential_ {};

        size_t thread_count_;
        size_t detail_level_;

        bool is_build;

      public:
        Builder();

        Builder &set_sequential(bool sequential) {
            sequential_ = sequential;
            return *this;
        }

        Builder &set_file_info(bool file_info) {
            file_info_ = file_info;
            return *this;
        }

        Builder &set_detail_level(size_t n) {
            detail_level_ = n;
            return *this;
        }

        Builder &set_thread_count(size_t n) {
            thread_count_ = n;
            return *this;
        }

        Builder &set_root_path(std::vector<std::string> &&dirs) {
            for (auto &i : dirs) {
                root_paths_.emplace_back(std::move(i));
            }
            return *this;
        }

        Builder &set_query(std::string query) {
            query_ = std::move(query);
            return *this;
        }

        Builder &set_replacement(std::string replacement) {
            this->replacement_ = std::move(replacement);
            use_replacement_ = true;
            return *this;
        };

        Builder &set_regex(bool regex) {
            this->use_regex_ = regex;
            return *this;
        };

        Builder &set_sensitive(bool sensitive) {
            case_sensitive_ = sensitive;
            return *this;
        };

        Builder &set_extensions(std::vector<std::string> exts) {
            extensions_ = std::move(exts);
            return *this;
        };

        Config build();
    };

  private:
    Config(
        std::vector<fs::path> &&root_path,
        std::vector<std::string> &&extensions,
        std::string &&query,
        std::string &&replacement,
        bool use_regex,
        bool case_sensitive,
        bool use_replacement,
        bool file_info,
        bool sequential_,
        size_t thread_count,
        size_t detail_level
    );
};

#endif // THREADPOOL_CONFIG_H
