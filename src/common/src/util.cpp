#include "util.h"

#include "rose/common/common_interface.h"

#include <sstream>


std::string
get_exe_dir() {
    char buffer[256] = {0};
    Rose::Common::get_bin_dir(buffer, 256);

    return std::string(buffer).append("/");
}

std::unordered_map<std::string, std::string>
Rose::Util::parse_args(int argc, char** argv) {
    std::unordered_map<std::string, std::string> res;

    for (int idx = 1; idx < argc; ++idx) {
        const int next_idx = idx + 1;
        if (next_idx < argc) {
            res.insert({(argv[idx]), argv[next_idx]});
        }
    }

    return res;
}