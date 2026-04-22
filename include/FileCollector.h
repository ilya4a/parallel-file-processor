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
    static std::vector<fs::path> collectRecursively(const fs::path& root,  const std::vector<std::string>& allowed_extensions) {
        std::vector<fs::path> result;

        if (allowed_extensions.empty()) {
            std::cout << "collectRecursively: allowed_extensions is empty" << std::endl;
            return result;
        }

        if (!fs::exists(root) || !fs::is_directory(root)) {
            std::cout << "dir does not exists" << std::endl;
            return result;
        }

        for (const auto& entry : fs::recursive_directory_iterator(root)) {

            fs::path ext = entry.path().extension();

            bool found = false;
            for (const auto& allowed : allowed_extensions) {
                if (ext == allowed) {
                    found = true;
                    break;
                }
            }
            if (!found) continue;
            result.push_back(entry.path());
        }
        return result;
    }
};


#endif //THREADPOOL_FILECOLLECTOR_H