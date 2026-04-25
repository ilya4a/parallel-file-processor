//
// Created by ilya on 4/22/26.
//

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

    file_result.search_result = search_result;
    was_search = true;

    return search_result;
}

FileResult FileProcessor::replace() {
    if (!was_search){ search(); was_search = true;}

    fs::path temp_file_name = create_temp_file();

    // ReplaceOptions replace_options(config.replacement(), temp_file_name, source, file_result.search_result);

    ReplaceOptions replace_options = ReplaceOptions::Builder(source, file_result.search_result)
    .set_replacement(config.replacement())
    .set_tmp_file_path(temp_file_name)
    .build();


    try {
        file_result.replace_result = TextReplacer::replace(replace_options);
    }catch (std::exception &e) {
        file_result.error_message = e.what();
        if (!replace_options.tmp_file_path().empty() && fs::exists(replace_options.tmp_file_path())) {
            std::error_code ec;
            fs::remove(replace_options.tmp_file_path(), ec);
        }
        std::cerr << "Replace error in " << file_path << ": " << e.what() << std::endl;
        return file_result;
    }

    std::filesystem::rename(temp_file_name, file_path);

    return file_result;
}
