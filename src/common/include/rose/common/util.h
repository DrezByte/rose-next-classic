#ifndef UTIL_H
#define UTIL_H
#pragma once

#include <string>
#include <unordered_map>

std::string get_exe_dir();

namespace Rose {
namespace Util {
std::unordered_map<std::string, std::string> parse_args(int argc, char** argv);

std::vector<std::string> split_string_whitespace(const std::string& s);
} // namespace Util
} // namespace Rose
#endif // UTIL_H