#include <iostream>

#include "include/App.h"
#include "include/Config.h"
#include "include/ThreadPool.h"

std::mutex cout_m;

int test_task(int id) {

   std::unique_lock<std::mutex> lock(cout_m);
   std::cout << "task " << id << " in thread: "
             << std::this_thread::get_id() << "\n";
   lock.unlock();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   return id*id;
}

void run_pool() {
    ThreadPool pool(4, 8);
    std::vector<std::future<int>> results;

    for (int i = 0; i < 20; ++i) {
        results.push_back(pool.add_task(test_task, i));
    }

    pool.wait_all();

    for (auto& i: results) {
        std::cout << i.get() << std::endl;
    }
}

#include "third_party/CLI11.hpp"

Config parse_cli(int argc, char* argv[]) {
    CLI::App app{"Multithreaded file search & replace utility"};

    std::vector<std::string> path_strings;
    std::string query;
    std::string replacement;
    std::vector<std::string> extensions;
    bool case_sensitive = false;
    int verbose = 0;
    bool json_output = false;
    int threads = 0;

    app.add_option("-p,--path", path_strings, "Root directories (can be repeated)")
        ->required()
        ->check(CLI::ExistingDirectory);

    app.add_option("-q,--query", query, "Search pattern")->required();

    app.add_option("-r,--replacement", replacement, "Replacement string");

    app.add_option("-e,--extensions", extensions, "File extensions (e.g. .cpp .h)");

    app.add_flag("-s,--case-sensitive", case_sensitive, "Case-sensitive search");

    app.add_option("-j,--threads", threads, "Number of threads (0=auto)");

    app.add_flag_function("-v", [&verbose](int count) {
        verbose = count;
    })->multi_option_policy(CLI::MultiOptionPolicy::Sum);


    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }

    Config::Builder builder;

    builder.set_query(std::move(query));

    if (!replacement.empty()) builder.set_replacement(std::move(replacement)); //

    builder.set_root_path(std::move(path_strings));

    if (!extensions.empty()) builder.set_extensions(std::move(extensions));

    if (case_sensitive) builder.set_sensitive(true);

    return builder.build();
}


int main() {
    Config config = Config::Builder()
    .set_root_path({"../test_dir2", "../test_dir"})
    .set_query("777777777777777777")
    .set_replacement("s...")
    .set_sensitive(false)
    .build();

    App app(config);
    app.run();

    return 0;
}

