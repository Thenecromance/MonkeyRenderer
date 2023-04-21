#pragma once

#include <algorithm>
#include <cstring>
#include <string>
// #include <crossguid/guid.hpp>
namespace utils::str {
std::string ToUpper(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

std::string ToLower(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

bool EndWith(std::string src, std::string str) {
  return (strstr(src.c_str(), str.c_str()) - src.c_str()) ==
         (strlen(src.c_str()) - strlen(str.c_str()));
}

inline std::string operator""_Upper(const char *str, std::size_t) {
  return ToUpper(str);
}

inline std::string operator""_Lower(const char *str, std::size_t) {
  return ToLower(str);
}

}  // namespace utils::str
