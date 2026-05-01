#include "ReplaceOptions.h"

ReplaceOptions::Builder::Builder(std::string_view content, const SearchResult &search_result)
    : content_(content),
      search_result_(search_result),
      replacement_size_ { 0 },
      is_build(false) { }

ReplaceOptions::Builder &ReplaceOptions::Builder::set_replacement(std::string replacement) {
    replacement_ = std::move(replacement);
    replacement_size_ = replacement_.size();
    return *this;
}

ReplaceOptions::Builder &ReplaceOptions::Builder::set_tmp_file_path(fs::path tmp_file_path) {
    tmp_file_path_ = std::move(tmp_file_path);
    if (tmp_file_path_.empty()) {
        throw std::runtime_error("ReplaceOptions: tmp_file_path is empty");
    }
    return *this;
}

ReplaceOptions ReplaceOptions::Builder::build() {
    if (tmp_file_path_.empty()) {
        throw std::runtime_error("ReplaceOptions: tmp_file_path is empty");
    }
    if (is_build) {
        throw std::runtime_error("ReplaceOptions: repeated call of build()");
    }

    ReplaceOptions replace_options(
        std::move(replacement_),
        std::move(tmp_file_path_),
        content_,
        search_result_,
        replacement_size_
    );

    is_build = true;
    return replace_options;
}

ReplaceOptions::ReplaceOptions(
    std::string &&replacement,
    fs::path &&tmp_file_path,
    std::string_view content,
    const SearchResult &search_result,
    size_t replacement_size
)
    : replacement_(std::move(replacement)),
      tmp_file_path_(std::move(tmp_file_path)),
      content_(content),
      search_result_(search_result),
      replacement_size_(replacement_size) { }
