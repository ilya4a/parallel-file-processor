#include <iostream>
#include <CLI11.hpp>
#include "App.h"
#include "Config.h"
#include "ThreadPool.h"

Config parse_cli(int argc, char* argv[]) {
    CLI::App app{"Multithreaded file search & replace utility"};

    app.footer(
        "Examples:\n"
        "thisgrep -p ./src -q \"TODO\" -v                     # Find all occurrences\n"
        "thisgrep -p . -q \"FIXME\" -vv                    # Verbose output (file‑level details)"
        "thisgrep -p ../dir/ -p ../dir2/ -q \"query\" -r \"replace\" -vvv"
    );

    std::vector<std::string> path_strings;
    std::string query;
    std::string replacement;
    std::vector<std::string> extensions;
    bool case_sensitive = false;
    int detail = 0;
    int threads = 0;

    app.add_option("-p,--path", path_strings, "Root directories (can be repeated)")
        ->required()
        ->check(CLI::ExistingDirectory);

    app.add_option("-q,--query", query, "Search pattern")->required();

    auto *opt_replacement = app.add_option("-r,--replacement", replacement, "Replacement string");

    app.add_option("-e,--extensions", extensions, "File extensions (e.g. .cpp .h)");

    app.add_flag("-s,--case-sensitive", case_sensitive, "Case-sensitive search");

    app.add_option("-j,--threads", threads, "Number of threads (0=auto)");

    app.add_flag_function("-v", [&detail](int count) {
        detail = count;
    }, "Increase output detail level (e.g. -v, -vv or -vvv )" )->multi_option_policy(CLI::MultiOptionPolicy::Sum);

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }

    Config::Builder builder;

    builder.set_root_path(std::move(path_strings));
    builder.set_query(std::move(query));


    if (*opt_replacement) builder.set_replacement(std::move(replacement));

    if (threads > 0) builder.set_thread_count(threads);
    if (detail > 0) builder.set_detail_level(detail);

    if (!extensions.empty()) builder.set_extensions(std::move(extensions));

    if (case_sensitive) builder.set_sensitive(true);

    return builder.build();
}


int main(int argc, char* argv[]) {

    Config config = parse_cli(argc, argv);

    // Config config = Config::Builder()
    // .set_root_path({"../test_dir2", "../test_dir"})
    // .set_query("77777")
    // .set_replacement("s...")
    // .set_sensitive(false)
    // .build();

    App app(config);
    app.run();

    return 0;
}

