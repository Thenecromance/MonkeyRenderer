#pragma once

#include <direct.h>
#include <sys/stat.h>

#include <fstream>
#include <sstream>
#include <string>

#include "Marcos/Marcos.hpp"

NSP_BGN(utils)
NSP_BGN(File)
/// @brief Get Current directory's Parent
/// @param Dir which u want to get
/// @return the result of the Directory
const std::string GetCurrentWorkingDirectory() {
  char cwd[1024];
  if (_getcwd(cwd, sizeof(cwd)) != NULL) {
    return cwd;
  }
  return "";
}

/// @brief Get Current directory's Parent
/// @param Dir which u want to get
/// @return the result of the Directory
const std::string GetParentDirectory(std::string Dir) {
  auto found = Dir.find_last_of("/\\");
  auto Parent = Dir.substr(0, found);
  return Parent;
}

/// @brief
/// @param Path
/// @return
bool FileExist(std::string Path) {
  bool bRet;
  std::ifstream file(Path.c_str());
  bRet = file.good();
  file.close();
  return bRet;
}

bool StringIsDirectory(std::string Path) {
  struct stat s;
  if (stat(Path.c_str(), &s) == 0) {
    if (s.st_mode & S_IFDIR) {
      return true;
    }
  }
  return false;
}

bool StringIsPath(std::string Path) {
  struct stat s;
  if (stat(Path.c_str(), &s) == 0) {
    if (s.st_mode & S_IFREG) {
      return true;
    }
  }
  return false;
}

/// @brief Read the file as string
/// @param FilePath
/// @return
std::string ReadAsString(std::string FilePath) {
  std::string file_data;
  std::ifstream ifs(FilePath);
  if (ifs.is_open()) {
    std::stringstream stream;
    stream << ifs.rdbuf();
    ifs.close();
    return stream.str();
  } else {
    printf("could not open the file %s \n", FilePath.c_str());
  }
  return file_data;
}

void *ReadAsData(std::string FilePath) {
  std::ifstream ifs(FilePath);
  if (ifs.good()) {
    ifs.seekg(0, std::ios::end);

    // std::fpos<_Mbstatet> p;
    int length = static_cast<int>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    char *buffer = new char[length];
    return buffer;
  }

  return nullptr;
}

/// @brief
/// @param str
/// @return
inline std::string operator""_RAS(const char *str, std::size_t) {
  return ReadAsString(str);
}

inline void *operator""_RAD(const char *str, std::size_t) {
  return ReadAsData(str);
}

inline bool operator""_IsPath(const char *str, std::size_t) {
  return StringIsPath(str);
}

inline bool operator""_IsDir(const char *str, std::size_t) {
  return StringIsDirectory(str);
}

inline std::string operator""_Parent(const char *str, std::size_t) {
  return GetParentDirectory(str);
}

NSP_END(File)
NSP_END(utils)
