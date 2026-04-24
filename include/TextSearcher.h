//
// Created by ilya on 4/24/26.
//

#ifndef THREADPOOL_TEXTSEARCHER_H
#define THREADPOOL_TEXTSEARCHER_H
#include <algorithm>
#include <string_view>

#include "results_structs.h"


class TextSearcher {
public:
    // static size_t count_utf8_symbols(std::string_view sv, size_t byte_pos) {
    //     size_t count = 0;
    //     for (size_t i = 0; i < byte_pos && i < sv.size(); ) {
    //         unsigned char c = sv[i];
    //         if (c < 0x80) i += 1;
    //         else if (c < 0xE0) i += 2;
    //         else if (c < 0xF0) i += 3;
    //         else i += 4;
    //         ++count;
    //     }
    //     return count;
    // }

    static size_t count_utf8_symbols(std::string_view sv, size_t byte_pos) {
        return byte_pos;
    }

    static std::pair<size_t, size_t> find_line_and_column (std::vector<size_t> const& line_starts, std::string_view const& content, size_t pos)  {
        auto it = std::upper_bound(line_starts.begin(), line_starts.end(), pos);
        size_t line = std::distance(line_starts.begin(), it) - 1;

        size_t line_start = *(it - 1);
        std::string_view line_content = content.substr(line_start, pos - line_start);
        size_t column = count_utf8_symbols(line_content, line_content.size());

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
                auto found = std::search(it, content.end(), pattern.begin(), pattern.end(),
                                         [](char a, char b) {
                                             return std::tolower(static_cast<unsigned char>(a)) ==
                                                    std::tolower(static_cast<unsigned char>(b));
                                         });

                found_pos = (found != content.end()) ? std::distance(content.begin(), found) : std::string_view::npos;
            }

            if (found_pos == std::string_view::npos) break;

            std::pair position = find_line_and_column(line_starts, content, found_pos);
            result.matches.emplace_back(position.first, position.second);

            search_pos = found_pos + 1;
        }
        return result;
    }
};


#endif //THREADPOOL_TEXTSEARCHER_H