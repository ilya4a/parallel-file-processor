//
// Created by ilya on 4/22/26.
//

#ifndef THREADPOOL_FILECOLLECTOR_H
#define THREADPOOL_FILECOLLECTOR_H
#include <vector>
#include <filesystem>


namespace fs = std::filesystem;

class FileCollector {
public:
    static std::vector<fs::path> collectRecursively(const fs::path& root) {
        std::vector<fs::path> result;

        if (!fs::exists(root) || !fs::is_directory(root)) {
            std::cout << "dir does not exists" << std::endl;
            return result;
        }

        for (const auto& entry : fs::recursive_directory_iterator(root)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            result.push_back(entry.path());
        }
        return result;
    }
};


#endif //THREADPOOL_FILECOLLECTOR_H