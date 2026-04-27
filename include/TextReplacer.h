
#ifndef THREADPOOL_TEXTREPLACER_H
#define THREADPOOL_TEXTREPLACER_H
#include "results_structs.h"
#include <string>
#include <filesystem>
#include <fstream>
#include <system_error>

#include "ReplaceOptions.h"

namespace fs = std::filesystem;

class TextReplacer {
public:
    static ReplaceResult replace(ReplaceOptions& options) {
        std::ofstream out(options.tmp_file_path(), std::ios::binary | std::ios::trunc);

        if (!out) {
            throw std::runtime_error("Failed to write to file: " + options.tmp_file_path().string());
        }

        size_t last_pos = 0;
        for (const auto &i: options.search_result().matches) {
            if (i.byte_pos < last_pos) {throw std::runtime_error("TextReplacer: Write failed");}
            out.write(options.content().data() + last_pos, i.byte_pos - last_pos);
            out.write(options.replacement().data(), options.replacement_size());
            last_pos = i.byte_pos + i.length;
        }

        out.write(options.content().data() + last_pos, options.content().size() - last_pos);

        if (!out) {
            throw std::runtime_error("TextReplacer: Write failed");
        }
        out.close();
        return ReplaceResult{options.search_result().matches.size()};
    }
};



#endif //THREADPOOL_TEXTREPLACER_H