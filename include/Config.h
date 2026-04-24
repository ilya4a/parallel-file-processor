//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_CONFIG_H
#define THREADPOOL_CONFIG_H

#include <string>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

struct Config {

private:
    fs::path root_path_;
    std::vector<std::string> extensions_;
    std::string query_;
    std::string replacement_;
    bool use_regex_;
    bool case_sensitive_;
    bool use_replacement_;

public:

    Config& set_replacement(std::string replacement) {
        this->replacement_ = std::move(replacement);
        use_replacement_ = true;
        return *this;
    };

    Config& set_regex(bool regex) {
        this->use_regex_ = regex;
        return *this;
    };


    Config& set_sensitive(bool sensitive) {
        case_sensitive_ = sensitive;
        return *this;
    };

    Config& set_extensions(std::vector<std::string> const& exts) {
        extensions_ = exts;
        return *this;
    };


    Config(fs::path path, std::string query) : root_path_(path), query_(query) {

        extensions_ = {".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hxx", ".inl",
        ".py", ".sh", ".bash", ".pl", ".rb", ".lua", ".js", ".ts",
        ".json", ".yaml", ".yml", ".toml", ".ini", ".cfg", ".conf", ".xml", ".properties", ".env",
        ".txt", ".md", ".rst", ".tex", ".log",
        ".cmake", ".mk", ".make",
        ".html", ".htm", ".css", ".scss", ".sass", ".less", ".svg",
        ".csv", ".tsv", ".sql", ".patch", ".diff"
        };

        use_regex_ = false;
        case_sensitive_ = false;
        replacement_ = "";
        use_replacement_ = false;
    }

    const fs::path& root_path() const { return root_path_; }
    const std::vector<std::string>& extensions() const { return extensions_; }
    const std::string& query() const { return query_; }
    const std::string& replacement() const { return replacement_; }
    bool use_regex() const { return use_regex_; }
    bool case_sensitive() const { return case_sensitive_; }
    bool use_replacement() const { return use_replacement_; }


};

#endif //THREADPOOL_CONFIG_H