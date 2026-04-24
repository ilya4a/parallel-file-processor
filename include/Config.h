//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_CONFIG_H
#define THREADPOOL_CONFIG_H

#include <string>
#include <vector>
#include <filesystem>
#include <cstddef>
namespace fs = std::filesystem;

struct Config {
    fs::path root_path;
    std::vector<std::string> extensions;
    std::string query;
    std::string replacement;
    bool use_regex;
    bool case_sensitive;

    Config(fs::path path, std::string query,
        std::string replacement = "",
        bool regex = false,
        bool sensitive = false,
        std::vector<std::string> exts = std::vector<std::string>()) : root_path(path), query(query) {
        if (exts.size() == 0) {
            extensions = {".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hxx", ".inl",
            ".py", ".sh", ".bash", ".pl", ".rb", ".lua", ".js", ".ts",
            ".json", ".yaml", ".yml", ".toml", ".ini", ".cfg", ".conf", ".xml", ".properties", ".env",
            ".txt", ".md", ".rst", ".tex", ".log",
            ".cmake", ".mk", ".make",
            ".html", ".htm", ".css", ".scss", ".sass", ".less", ".svg",
            ".csv", ".tsv", ".sql", ".patch", ".diff"
            };
        }else {
            extensions = std::move(exts);
        }

        this->replacement = replacement;

        use_regex = regex;
        case_sensitive = sensitive;
    };

};

#endif //THREADPOOL_CONFIG_H