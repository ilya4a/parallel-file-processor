#include "Config.h"

Config::Builder::Builder() {
    extensions_ = { ".c",    ".cpp",   ".cc",   ".cxx",  ".h",          ".hpp", ".hxx",  ".inl",  ".py",   ".sh",
                    ".bash", ".pl",    ".rb",   ".lua",  ".js",         ".ts",  ".json", ".yaml", ".yml",  ".toml",
                    ".ini",  ".cfg",   ".conf", ".xml",  ".properties", ".env", ".txt",  ".md",   ".rst",  ".tex",
                    ".log",  ".cmake", ".mk",   ".make", ".html",       ".htm", ".css",  ".scss", ".sass", ".less",
                    ".svg",  ".csv",   ".tsv",  ".sql",  ".patch",      ".diff" };

    replacement_ = "";

    use_regex_ = false;
    case_sensitive_ = false;
    use_replacement_ = false;
    file_info_ = false;
    bool sequential_ = false;

    is_build = false;

    thread_count_ = 0;
    detail_level_ = 1;
}

Config Config::Builder::build() {
    if (root_paths_.empty() || query_.empty()) {
        throw std::runtime_error("Config: invalid Builder call");
    }

    if (is_build) {
        throw std::runtime_error("Config: repeated call of build");
    }

    Config config(
        std::move(root_paths_),
        std::move(extensions_),
        std::move(query_),
        std::move(replacement_),
        use_regex_,
        case_sensitive_,
        use_replacement_,
        file_info_,
        sequential_,
        thread_count_,
        detail_level_
    );

    is_build = true;
    return config;
}

Config::Config(
    std::vector<fs::path> &&root_path,
    std::vector<std::string> &&extensions,
    std::string &&query,
    std::string &&replacement,
    bool use_regex,
    bool case_sensitive,
    bool use_replacement,
    bool file_info,
    bool sequential_,
    size_t thread_count,
    size_t detail_level
)
    : root_paths_(std::move(root_path)),
      extensions_(std::move(extensions)),
      query_(std::move(query)),
      replacement_(std::move(replacement)),
      use_regex_(use_regex),
      case_sensitive_(case_sensitive),
      use_replacement_(use_replacement),
      file_info_(file_info),
      sequential_(sequential_),
      thread_count_(thread_count),
      detail_level_(detail_level) { }
