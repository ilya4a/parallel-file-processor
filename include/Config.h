#ifndef THREADPOOL_CONFIG_H
#define THREADPOOL_CONFIG_H

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

struct Config {

    const std::vector<fs::path>& root_paths() const { return root_paths_; }
    const std::vector<std::string>& extensions() const { return extensions_; }
    const std::string& query() const { return query_; }
    const std::string& replacement() const { return replacement_; }
    bool use_regex() const { return use_regex_; }
    bool case_sensitive() const { return case_sensitive_; }
    bool use_replacement() const { return use_replacement_; }
    bool file_info() const { return file_info_; }

    size_t thread_count() const { return thread_count_; }
    size_t detail_level() const { return detail_level_; }


private:
    std::vector<fs::path> root_paths_;
    std::vector<std::string> extensions_;
    std::string query_;
    std::string replacement_;
    bool use_regex_;
    bool case_sensitive_;
    bool use_replacement_;
    bool file_info_;
    size_t thread_count_;
    size_t detail_level_;

public:

    class Builder {
        std::vector<fs::path> root_paths_;
        std::vector<std::string> extensions_;
        std::string query_;
        std::string replacement_;
        bool use_regex_;
        bool case_sensitive_;
        bool use_replacement_;
        bool file_info_;

        size_t thread_count_;
        size_t detail_level_;

        bool is_build;

    public:

        Builder() {
            extensions_ = {".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hxx", ".inl",
                ".py", ".sh", ".bash", ".pl", ".rb", ".lua", ".js", ".ts",
                ".json", ".yaml", ".yml", ".toml", ".ini", ".cfg", ".conf", ".xml", ".properties", ".env",
                ".txt", ".md", ".rst", ".tex", ".log",
                ".cmake", ".mk", ".make",
                ".html", ".htm", ".css", ".scss", ".sass", ".less", ".svg",
                ".csv", ".tsv", ".sql", ".patch", ".diff"
                };

            replacement_ = "";

            use_regex_ = false;
            case_sensitive_ = false;
            use_replacement_ = false;
            file_info_ = false;

            is_build = false;

            thread_count_ = 0;
            detail_level_ = 1;
        }

        Builder& set_file_info(bool file_info) {
            file_info_ = file_info;
            return *this;
        }

        Builder& set_detail_level(size_t n) {
            detail_level_ = n;
            return *this;
        }

        Builder& set_thread_count(size_t n) {
            thread_count_ = n;
            return *this;
        }

        Builder& set_root_path(std::vector<std::string>&& dirs) {

            for (auto &i: dirs) {
                root_paths_.emplace_back(std::move(i));
            }
            return *this;
        }

        Builder& set_query(std::string query) {
            query_ = std::move(query);
            return *this;
        }

        Builder& set_replacement(std::string replacement) {
            this->replacement_ = std::move(replacement);
            use_replacement_ = true;
            return *this;
        };

        Builder& set_regex(bool regex) {
            this->use_regex_ = regex;
            return *this;
        };

        Builder& set_sensitive(bool sensitive) {
            case_sensitive_ = sensitive;
            return *this;
        };

        Builder& set_extensions(std::vector<std::string> exts) {
            extensions_ = std::move(exts);
            return *this;
        };

        Config build() {

            if (root_paths_.empty() || query_.empty()) throw std::runtime_error("Config: incorrect call of builder");

            if (is_build) throw std::runtime_error("Config: repeated call of build");

            Config config(std::move(root_paths_),
                std::move(extensions_),
                std::move(query_),
                std::move(replacement_),
                use_regex_,
                case_sensitive_,
                use_replacement_,
                file_info_,
                thread_count_,
                detail_level_);

            is_build = true;
            return config;
        }

    };

private:
    Config(
    std::vector<fs::path>&& root_path,
    std::vector<std::string>&& extensions,
    std::string&& query,
    std::string&& replacement,
    bool use_regex,
    bool case_sensitive,
    bool use_replacement,
    bool file_info,
    size_t thread_count,
    size_t detail_level
    ) :

    root_paths_(std::move(root_path)),
    extensions_(std::move(extensions)),
    query_(std::move(query)),
    replacement_(std::move(replacement)),
    use_regex_(use_regex),
    case_sensitive_(case_sensitive),
    use_replacement_(use_replacement),
    file_info_(file_info),
    thread_count_(thread_count),
    detail_level_(detail_level){
    }
};

#endif //THREADPOOL_CONFIG_H