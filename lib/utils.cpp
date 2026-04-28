//
// Created by ilya on 4/28/26.
//

#include "utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include "ReplaceOptions.h"

namespace utils {


    void print_line_with_caret_under_line(const std::string& line, size_t line_num, const std::vector<size_t>& starts, size_t length){

        std::string prefix = std::to_string(line_num) + ": ";
        std::cout << prefix << line << '\n';

        std::string underline(prefix.size() + line.size(), ' ');

        for (size_t start : starts) {
            size_t underline_start = prefix.size() + start;
            size_t underline_end = std::min(underline_start + length, underline.size());
            for (size_t i = underline_start; i < underline_end; ++i) {
                underline[i] = '_';
            }
        }
        std::cout << underline << '\n';
    }

    void print_line_from_file(const fs::path& filePath, std::vector<Match>& matches, size_t match_len) {
        if (match_len == 0) match_len = matches[0].length;

        // std::cout << "DEBUG line=" << match.line
        //   << " byte_pos=" << match.byte_pos
        //   << " column_bytes=" << match.column_bytes
        //   << " length=" << match.length
        //   << " match.lenght=" << match.length
        //   << " line=[" << match.line << "]" << std::endl;

        if (!std::filesystem::exists(filePath)) {
            throw std::runtime_error( "printLineFromFile: file does not exists: " + filePath.string());
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error( "printLineFromFile: cannot open file:  " + filePath.string());
        }

        std::string line;
        size_t currentLine = 0;
        while (std::getline(file, line)) {
            if (currentLine == matches[0].line) {
                std::vector<size_t> starts(matches.size());
                for (size_t i = 0; i < matches.size(); i++) starts[i] = matches[i].column_bytes;
                print_line_with_caret_under_line(line, matches[0].line, starts, match_len);
                return;
            }
            ++currentLine;
        }
        file.close();
    }

    std::vector<fs::path> collectFilesRecursively(const fs::path& root,  const std::vector<std::string>& allowed_extensions) {
        std::vector<fs::path> result;

        if (allowed_extensions.empty()) {
            std::cout << "collectFilesRecursively: allowed_extensions is empty" << std::endl;
            return result;
        }

        if (!fs::exists(root) || !fs::is_directory(root)) {
            std::cout << "collectFilesRecursively: dir does not exists" << std::endl;
            return result;
        }

        for (const auto& entry : fs::recursive_directory_iterator(root)) {

            fs::path ext = entry.path().extension();

            bool found = false;
            for (const auto& allowed : allowed_extensions) {
                if (ext == allowed) {
                    found = true;
                    break;
                }
            }
            if (!found) continue;
            result.push_back(entry.path());
        }
        return result;
    }


    ReplaceResult replace(ReplaceOptions& options) {
        std::ofstream out(options.tmp_file_path(), std::ios::binary | std::ios::trunc);

        if (!out) {
            throw std::runtime_error("Failed to write to file: " + options.tmp_file_path().string());
        }

        size_t last_pos = 0;
        for (const auto &i: options.search_result().matches) {
            if (i.byte_pos < last_pos) {throw std::runtime_error("TextReplacer: Write failed");}
            out.write(options.content().data() + last_pos, i.byte_pos - last_pos);
            out.write(options.replacement().data(), options.replacement_size());
            last_pos = i.byte_pos + i.length;
        }

        out.write(options.content().data() + last_pos, options.content().size() - last_pos);

        if (!out) {
            throw std::runtime_error("TextReplacer: Write failed");
        }
        out.close();
        return ReplaceResult{options.search_result().matches.size()};
    }


    std::pair<size_t, size_t> find_line_and_column (std::vector<size_t> const& line_starts, size_t pos)  {
        auto it = std::upper_bound(line_starts.begin(), line_starts.end(), pos);
        size_t line = std::distance(line_starts.begin(), it) - 1;

        size_t line_start = *(it - 1);
        return {line, pos - line_start};
    };

    SearchResult search(std::string_view content, const SearchOptions& options) {
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

            search_pos = found_pos + pattern.size();
        }
        return result;
    }

}
