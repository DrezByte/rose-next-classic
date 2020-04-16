#include "util.h"

#include "rose/common/common_interface.h"

#include <iterator>
#include <sstream>

std::string
get_exe_dir() {
    char buffer[256] = {0};
    Rose::Common::get_bin_dir(buffer, 256);

    return std::string(buffer).append("/");
}

namespace Rose::Util {
std::unordered_map<std::string, std::string>
parse_args(int argc, char** argv) {
    std::unordered_map<std::string, std::string> res;

    for (int idx = 1; idx < argc; ++idx) {
        const int next_idx = idx + 1;
        if (next_idx < argc) {
            res.insert({(argv[idx]), argv[next_idx]});
        }
    }

    return res;
}

std::vector<std::string>
split_string_whitespace(const std::string& s) {
    std::istringstream iss(s);
    return std::vector<std::string>{std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}};
}
}; // namespace Rose::Util
