#include "util.h"

#include "rose/common/common_interface.h"

std::string
get_exe_dir() {
    char buffer[256] = {0};
    Rose::Common::get_bin_dir(buffer, 256);

    return std::string(buffer).append("/");
}