#include "App.h"
#include <sys/ioctl.h>
#include "utils.h"

App::App(Config config) : conf(config){}

size_t get_terminal_width() {
    winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        if (w.ws_col > 0) {
            return w.ws_col;
        }
    }
    return 80;
}


void print_line_with_caret_under_line(const std::string& line, size_t line_num, const std::vector<size_t>& starts, size_t length){

    std::string prefix = std::to_string(line_num) + ": ";
    std::cout << prefix << line << '\n';

    std::string underline(prefix.size() + line.size(), ' ');

    // std::cout << "DEBUG underline: line.size=" << line.size()
    //       << " starts.size=" << starts.size() << " length=" << length << '\n';
    // for (size_t start : starts) {
    //     std::cout << "  start=" << start
    //               << " underline_start=" << prefix.size() + start
    //               << " underline_end=" << std::min(prefix.size() + start + length, underline.size())
    //               << '\n';
    // }

    for (size_t start : starts) {
        size_t underline_start = prefix.size() + start;
        size_t underline_end = std::min(underline_start + length, underline.size());
        for (size_t i = underline_start; i < underline_end; ++i) {
            underline[i] = '^';
        }
    }
    std::cout << underline << '\n';
}

void print_line_from_file(const fs::path& filePath, std::vector<Match>& matches, size_t match_len = 0) {
    if (match_len == 0) match_len = matches[0].length;

    // std::cout << "DEBUG line=" << matches[0].line
    //   << " byte_pos=" << matches[0].byte_pos
    //   << " column_bytes=" << matches[0].column_bytes
    //   << " length=" << matches[0].length
    //   << " match_len=" << match_len
    //   << " match.lenght=" << matches[0].length
    //   << " line=[" << matches[0].line << "]" << std::endl;

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

void print_info(FileResult& file_result, size_t max_w_time = 0, size_t max_w_words = 0, size_t max_w_size = 0, size_t max_w_lines = 0){

    if (max_w_time == 0)  max_w_time  = std::to_string(file_result.processing_time_us).size();
    if (max_w_words == 0) max_w_words = std::to_string(file_result.search_result.total_words).size();
    if (max_w_size == 0) max_w_words = std::to_string(file_result.search_result.total_bytes).size();
    if (max_w_lines == 0) max_w_words = std::to_string(file_result.search_result.lines).size();

    std::string line(get_terminal_width(), '-');
    std::cout << line << '\n';

    std::cout << "time: "        << std::setw(max_w_time)  << file_result.processing_time_us << " microseconds"
              << " | total_words: " << std::setw(max_w_words) << file_result.search_result.total_words
              << " | total_size: "  << std::setw(max_w_size)  << file_result.search_result.total_bytes
              << " | total_lines: " << std::setw(max_w_lines) << file_result.search_result.lines
              << '\n';

}


void App::print_detail_level3(FileResult& file_result) {

    if (file_result.search_result.matches.size() > 0) {

        std::cout  << "[" << file_result.file_path << "]" << std::endl;
        std::cout << "found: " << file_result.search_result.matches.size();
        if (file_result.replace_result.num_replacements > 0) {
            std::cout << " replaces" << std::endl;
        }else {
            std::cout << " matches" << std::endl;
        }

        size_t matches_count = file_result.search_result.matches.size();

        for (size_t i = 0; i < matches_count; i++) {
            std::vector<Match> matches_in_line;

            if (i != matches_count - 1) {

                matches_in_line.push_back(file_result.search_result.matches[i]);
                size_t j = 1;
                while (file_result.search_result.matches[i].line == file_result.search_result.matches[i + 1].line) {
                    matches_in_line.push_back(file_result.search_result.matches[i + 1]);

                    if (file_result.replace_result.num_replacements > 0) {
                        matches_in_line.back().column_bytes += (file_result.replace_result.bytes - file_result.search_result.matches[0].length) * j;
                    }

                    i++;
                    j++;
                    if (i + 1 >= matches_count) break;
                }
            }else {
                matches_in_line.push_back(file_result.search_result.matches[i]);
            }

            if (file_result.replace_result.num_replacements > 0) {
                print_line_from_file(file_result.file_path, matches_in_line, file_result.replace_result.bytes);
            }else {
                print_line_from_file(file_result.file_path, matches_in_line);
            }
        }

        if (conf.file_info()) {
            print_info(file_result);
        }

        std::string line2(get_terminal_width(), '=');
        std::cout << line2 << '\n';
    }
    // std::cout << std::endl;

}


void App::handle_results(std::vector<std::future<FileResult>> &results_future)  {

    std::vector<FileResult> results(results_future.size());
    int i = 0;
    for (auto& fr : results_future) {
        results[i] = fr.get();
        i++;
    }

    size_t matches_total = 0; size_t words_total = 0; size_t bytes_total = 0; size_t lines_total = 0;

    size_t max_w_time = 0, max_w_words = 0, max_w_size = 0, max_w_lines = 0;

    for (auto& fr : results) {
        max_w_time  = std::max(max_w_time,  std::to_string(fr.processing_time_us).size());
        max_w_words = std::max(max_w_words, std::to_string(fr.search_result.total_words).size());
        max_w_size  = std::max(max_w_size,  std::to_string(fr.search_result.total_bytes).size());
        max_w_lines = std::max(max_w_lines, std::to_string(fr.search_result.lines).size());
    }

    for (auto &file_result: results) {

        if (conf.detail_level() == 1) {

        }else if (conf.detail_level() == 2) {
            if (file_result.search_result.matches.size() > 0) {
                std::cout  << "[" << file_result.file_path << "]" << std::endl;
                std::cout << "found: " << file_result.search_result.matches.size() << std::endl;

                if (conf.file_info()) {
                    print_info(file_result, max_w_time, max_w_words, max_w_size, max_w_lines);
                }
                std::string line2(get_terminal_width(), '=');
                std::cout << line2 << '\n';
            }

        }else if (conf.detail_level() == 3) {
            print_detail_level3(file_result);
        }

        matches_total += file_result.search_result.matches.size();
        words_total += file_result.search_result.total_words;
        bytes_total += file_result.search_result.total_bytes;
        lines_total += file_result.search_result.lines;
    }


    if (conf.use_replacement()) {
        std::cout << "replaces total: " << matches_total << std::endl;
    }else {
        std::cout << "matches total: " << matches_total << std::endl;
    }

    if (conf.file_info()) {
        std::cout << "words total: " << words_total
        << " | bytes total: " << bytes_total
        << " | lines total: " << lines_total << std::endl;
    }
}


void App::run() {

    auto start = std::chrono::steady_clock::now();

    ThreadPool thread_pool{conf.thread_count()};
    std::vector<std::future<FileResult>> results;

    for (auto& i : conf.root_paths() ) {

        std::vector<fs::path> files = utils::collectFilesRecursively(i, conf.extensions());

        for (auto const & path: files) {
            FileProcessor file_processor(path, conf);

            results.push_back(thread_pool.add_task([this, path] () -> FileResult {
                FileProcessor file_processor(path, conf);
                return file_processor.process_file();
            }));
        }
    }

    thread_pool.wait_all();
    auto end = std::chrono::steady_clock::now();
    size_t processing_time_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    handle_results(results);

    if (conf.file_info()) {
        std::cout << "TOTAL TIME: " << processing_time_us << " microseconds" << std::endl;
    }
}




