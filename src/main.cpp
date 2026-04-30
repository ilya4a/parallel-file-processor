#include <iostream>
#include "CLI/CLI.hpp"

#include "App.h"4
#include "Config.h"
#include "ThreadPool.h"

Config parse_cli(int argc, char *argv[]) {
    CLI::App app{"Multithreaded file search & replace utility"};
    app.name("frep");

    app.footer(
        "Examples:\n"
        "  frep -p /path/to/dir /another/file -q \"TODO\" -v               # find all occurrences, brief output\n"
        "  frep -p /path -q \"TODO\" -vv -i                               # verbose file‑level details + statistics (time, words)\n"
        "  frep -p /path -q \"TODO\" -r \"replace\"                       # search and replace, default brief output\n"
        "  frep -p /path -q \"TODO\" -r \"replace\" -vvv -i               # full output with underline for each match/replace + statistics"
    );

    std::vector<std::string> path_strings;
    std::string query;
    std::string replacement;
    std::string extensions;
    bool case_sensitive = false;
    bool file_info = false;
    bool sequential = false;
    int detail = 0;
    int threads = 0;

    app.add_option("-p,--path", path_strings, "Root directories (can be repeated)")
            ->required()
            ->check(CLI::ExistingPath);

    app.add_option("-q,--query", query, "Search pattern")->required();

    auto *opt_replacement = app.add_option("-r,--replacement", replacement,
                                           "Replacement string (if omitted, search only)");

    app.add_option("-e,--extensions", extensions, "File extensions (e.g. .cpp .h)");

    app.add_flag("-s,--case-sensitive", case_sensitive, "Case-sensitive search");

    app.add_flag("-i,--file-info", file_info, "Show per‑file statistics (time, word count, etc.)");

    app.add_flag("-S,--sequential", sequential,
                 "Run sequentially (single‑threaded, default: parallel with auto threads)");

    app.add_option("-j,--threads", threads, "Number of threads (0=auto)");

    app.add_flag_function("-v", [&detail](int count) {
        detail = count;
    }, "Increase output detail level (e.g. -v, -vv or -vvv )")->multi_option_policy(CLI::MultiOptionPolicy::Sum);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        std::exit(app.exit(e));
    }

    Config::Builder builder;

    builder.set_root_path(std::move(path_strings));
    builder.set_query(std::move(query));


    if (*opt_replacement) builder.set_replacement(std::move(replacement));

    if (threads > 0) builder.set_thread_count(threads);
    if (detail > 0) builder.set_detail_level(detail);

    std::vector<std::string> extensions_vector;

    std::istringstream iss(extensions);
    std::string ext;
    while (iss >> ext) {
        extensions_vector.push_back(ext);
    }

    if (!extensions.empty()) builder.set_extensions(std::move(extensions_vector));

    if (case_sensitive) builder.set_sensitive(true);
    if (file_info) builder.set_file_info(true);
    if (sequential) builder.set_sequential(true);

    return builder.build();
}


int main(int argc, char *argv[]) {
    Config config = parse_cli(argc, argv);

    App app(config);
    if (!config.sequential()) {
        app.run();
    } else {
        app.run_sequentially();
    }

    return 0;
}
