#include "App.h"
#include <sys/ioctl.h>
#include "utils.h"

App::App(Config config) : conf(config){}

size_t get_terminal_width() {
    winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
        return w.ws_col;
    return 80;
}

void App::print_detail_level3(FileResult& file_result) {

    if (file_result.search_result.matches.size() > 0) {

        std::cout  << file_result.file_path << std::endl;
        std::cout << "found: " << file_result.search_result.matches.size() << " matches" << std::endl;

        size_t matches_count = file_result.search_result.matches.size();

        for (size_t i = 0; i < matches_count; i++) {
            std::vector<Match> matches_in_line;

            if (i != matches_count - 1) {

                matches_in_line.push_back(file_result.search_result.matches[i]);
                size_t j = 1;
                while (file_result.search_result.matches[i].line == file_result.search_result.matches[i + 1].line) {
                    matches_in_line.push_back(file_result.search_result.matches[i + 1]);
                    matches_in_line.back().column_bytes += (file_result.replace_result.bytes - file_result.search_result.matches[0].length) * j;
                    i++;
                    j++;
                    if (i + 1 >= matches_count) break;
                }
            }else {
                matches_in_line.push_back(file_result.search_result.matches[i]);
            }

            if (file_result.replace_result.num_replacements > 0) {
                utils::print_line_from_file(file_result.file_path, matches_in_line, file_result.replace_result.bytes);
            }else {
                utils::print_line_from_file(file_result.file_path, matches_in_line);
            }
        }

        if (conf.file_info()) {
            std::string info = "";

            info +="time:  " + std::to_string(file_result.processing_time_us) + " microseconds";
            info += " total_words:  " + std::to_string(file_result.search_result.total_words);
            info +=  " total_size:  " + std::to_string(file_result.search_result.total_bytes);
            info += " total_lines:  " + std::to_string(file_result.search_result.lines);

            std::cout << info << std::endl;
            std::string line(get_terminal_width(), '_');
            std::cout << line << std::endl;
        }
    }

    std::cout << std::endl;
}

// info +="time:  " + file_result.processing_time_us + " microseconds";
// std::cout << " total_words:  " << file_result.search_result.total_words;
// std::cout << " total_size:  " << file_result.search_result.total_bytes;
// std::cout << " total_lines:  " << file_result.search_result.lines << std::endl;

void App::handle_results(std::vector<std::future<FileResult>> &results)  {

    size_t matches_total = 0;
    size_t words_total = 0;
    size_t bytes_total = 0;
    size_t lines_total = 0;

    for (auto &i: results) {

        FileResult file_result = i.get();

        if (conf.detail_level() == 1) {

        }else if (conf.detail_level() == 2) {
            std::cout  << file_result.file_path << std::endl;
            std::cout << "total_words:  " << file_result.search_result.total_words << std::endl;
            std::cout << "found: " << file_result.search_result.matches.size() << " matches" << std::endl;
            std::cout << std::endl;
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
    std::cout << "words total: " << words_total << std::endl;
    std::cout << "bytes total: " << bytes_total << std::endl;
    std::cout << "lines total: " << lines_total << std::endl;
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

    std::cout << "TOTAL TIME: " << processing_time_us << " microseconds" << std::endl;
}




