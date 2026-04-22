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


namespace fs = std::filesystem;


#include <string_view>
#include <algorithm>

class TextSearcher {
public:

    static std::pair<size_t, size_t> find_line_and_column (std::vector<size_t> const& line_starts, size_t pos)  {
        auto it = std::upper_bound(line_starts.begin(), line_starts.end(), pos);
        size_t line = std::distance(line_starts.begin(), it) - 1;
        size_t column = pos - *(it - 1);
        return {line, column};
    };

    static SearchResult search(std::string_view content, const SearchOptions& options) {
        SearchResult result;

        const std::string_view pattern = options.find;
        if (pattern.empty()) return result;

        std::vector<size_t> line_starts = {0};
        for (size_t i = 0; i < content.size(); ++i) {
            if (content[i] == '\n') {
                line_starts.push_back(i + 1);
            }
        }

        size_t search_pos = 0;
        while (true) {
            size_t found_pos;

            if (options.case_sensitive) {
                found_pos = content.find(pattern, search_pos);
            } else {
                auto it = content.begin() + search_pos;
                const char* found = std::search(it, content.end(), pattern.begin(), pattern.end(),
                                         [](char a, char b) {
                                             return std::tolower(static_cast<unsigned char>(a)) ==
                                                    std::tolower(static_cast<unsigned char>(b));
                                         });

                found_pos = (found != content.end()) ? std::distance(content.begin(), found) : std::string_view::npos;
            }

            if (found_pos == std::string_view::npos) break;

            std::pair position = find_line_and_column(line_starts, found_pos);
            result.matches.emplace_back(position.first, position.second);

            search_pos = found_pos + 1;
        }

        return result;
    }
};

class TextReplacer {
public:
    static ReplaceResult replace(std::string_view content,
                               std::string_view replacement, ReplaceOptions const& options){}
};

class FileProcessor {
    Config const& config;
    fs::path const& file_path;

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

    FileProcessor(fs::path const& path, Config const& conf) :config(conf), file_path(path) {}

    FileResult get_file_result() {
        FileResult res;
        std::string sourse = readFileToString(file_path);

        SearchOptions options(config.query);

        res.search_result = TextSearcher::search(sourse, options);

        return res;
    }


};


#endif //THREADPOOL_FILEPROCECCOR_H