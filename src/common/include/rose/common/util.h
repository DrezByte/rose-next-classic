#ifndef UTIL_H
#define UTIL_H
#pragma once

#include <string>
#include "rose/common/common_interface.h"

std::string
get_exe_dir() {
    char buffer[256] = {0};
    Rose::Common::get_bin_dir(buffer, 256);

    return std::string(buffer).append("/");
}

#endif // UTIL_H