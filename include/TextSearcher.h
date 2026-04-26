

#ifndef THREADPOOL_TEXTSEARCHER_H
#define THREADPOOL_TEXTSEARCHER_H
#include <algorithm>
#include <string_view>

#include "results_structs.h"


class TextSearcher {
public:

    static std::pair<size_t, size_t> find_line_and_column (std::vector<size_t> const& line_starts, size_t pos)  {
        auto it = std::upper_bound(line_starts.begin(), line_starts.end(), pos);
        size_t line = std::distance(line_starts.begin(), it) - 1;

        size_t line_start = *(it - 1);
        return {line, pos - line_start};
    };

    static SearchResult search(std::string_view content, const SearchOptions& options) {
        SearchResult result;

        const std::string_view pattern = options.find;
        if (pattern.empty()) return result;

        std::vector<size_t> line_starts = {0};

        size_t word_count = 0;
        bool in_word = false;

        for (size_t i = 0; i < content.size(); ++i) {

            auto c = static_cast<unsigned char>(content[i]);

            if (std::isspace(c)) {
                in_word = false;
            } else {
                if (!in_word) {
                    ++word_count;
                    in_word = true;
                }
            }

            if (content[i] == '\n') {
                line_starts.push_back(i + 1);
            }
        }

        result.lines = line_starts.size();
        result.total_bytes = content.size();
        result.total_words = word_count;

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

            std::pair position = find_line_and_column(line_starts, found_pos);

            result.matches.emplace_back(position.first, position.second, found_pos);

            result.matches.back().length = options.find.size(); //regex fix

            search_pos = found_pos + 1;
        }
        return result;
    }
};


#endif //THREADPOOL_TEXTSEARCHER_H