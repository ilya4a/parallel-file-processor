

#include "../include/FileProcessor.h"
#include <unistd.h>
#include "../include/TextSearcher.h"
#include "../include/TextReplacer.h"

std::string FileProcessor::readFileToString(const std::filesystem::path &path){
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

std::string FileProcessor::create_temp_file() {
    std::string tmpl = file_path.string() + ".tmp.XXXXXX";
    int fd = mkstemp(tmpl.data());
    if (fd == -1) {
        throw std::runtime_error("mkstemp failed");
    }
    close(fd);
    return tmpl;
}

SearchResult FileProcessor::search()  {

    source = readFileToString(file_path);

    SearchOptions options(config.query(), config.use_regex(), config.case_sensitive());

    SearchResult search_result = TextSearcher::search(source, options);

    return search_result;
}

ReplaceResult FileProcessor::replace(SearchResult const& search_result) {

    ReplaceResult replace_result{0};
    fs::path temp_file_name;

    try {

        temp_file_name =create_temp_file();

        ReplaceOptions replace_options = ReplaceOptions::Builder(source, search_result)
        .set_replacement(config.replacement())
        .set_tmp_file_path(temp_file_name)
        .build();

        replace_result = TextReplacer::replace(replace_options);

    }catch (std::exception &e) {
        replace_result.error_massage = "Replace error in file" + file_path.string() + " : " + e.what();

        if (!temp_file_name.empty() && fs::exists(temp_file_name)) {
            std::error_code ec;
            fs::remove(temp_file_name, ec);
        }

        std::cerr << "Replace error in " << file_path << ": " << e.what() << std::endl;
        return replace_result;
    }

    std::filesystem::rename(temp_file_name, file_path);

    return replace_result;
}

FileResult FileProcessor::process_file() {

    auto start = std::chrono::steady_clock::now();

    SearchResult search_result = search();
    FileResult file_result;

    if (config.use_replacement()) {
        ReplaceResult replace_result = replace(search_result);
        file_result.replace_result = std::move(replace_result);
    }

    file_result.search_result = std::move(search_result);
    auto end = std::chrono::steady_clock::now();

    file_result.processing_time_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return file_result;
}
