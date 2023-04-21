
#include "File.hpp"

#include <filesystem>
#include <fstream>
#include <utility>

#pragma region File

File::File(std::string sPath) : sPath(std::move(sPath)) {
  if (exists()) set_magic_num();
}

bool File::read(std::vector<std::string> &vLines) {
  std::ifstream file(sPath);
  if (!file.is_open()) {
    return false;
  }

  std::string sLine;
  while (std::getline(file, sLine)) {
    vLines.push_back(sLine);
  }

  file.close();
  return true;
}

bool File::read(std::string &sContent) {
  std::ifstream file(sPath);
  if (!file.is_open()) {
    return false;
  }

  std::string sLine;
  while (std::getline(file, sLine)) {
    sContent += sLine;
  }

  file.close();
  return true;
}

bool File::read(std::vector<char> &vContent) {
  std::ifstream file(sPath, std::ios::binary);
  if (!file.is_open()) {
    return false;
  }

  file.seekg(0, std::ios::end);
  vContent.resize(file.tellg());
  file.seekg(0, std::ios::beg);

  file.read(vContent.data(), vContent.size());

  file.close();
  return true;
}

bool File::write(const std::vector<std::string> &vLines) {
  std::ofstream file(sPath);
  if (!file.is_open()) {
    return false;
  }

  for (const auto &sLine : vLines) {
    file << sLine << std::endl;
  }

  file.close();
  return true;
}


bool File::write(const std::string &sContent) {
  std::ofstream file(sPath);
  if (!file.is_open()) {
    return false;
  }

  file << sContent;

  file.close();
  return true;
}

bool File::write(const std::vector<char> &vContent) {
  std::ofstream file(sPath, std::ios::binary);
  if (!file.is_open()) {
    return false;
  }

  file.write(vContent.data(), vContent.size());

  file.close();
  return true;
}

bool File::append(const std::vector<std::string> &vLines) {
  std::ofstream file(sPath, std::ios::app);
  if (!file.is_open()) {
    return false;
  }

  for (const auto &sLine : vLines) {
    file << sLine << std::endl;
  }

  file.close();
  return true;
}

bool File::append(const std::string &sContent) {
  std::ofstream file(sPath, std::ios::app);
  if (!file.is_open()) {
    return false;
  }

  file << sContent;

  file.close();
  return true;
}

bool File::append(const std::vector<char> &vContent) {
  std::ofstream file(sPath, std::ios::app | std::ios::binary);
  if (!file.is_open()) {
    return false;
  }

  file.write(vContent.data(), vContent.size());

  file.close();
  return true;
}

bool File::remove() {
  if (exists()) return std::filesystem::remove(sPath);
  return false;
}

bool File::rename(const std::string &sNewName) {
  if (exists()) std::filesystem::rename(sPath, sNewName);
  return true;
}

bool File::copy(const std::string &sNewPath) {
  if (exists()) std::filesystem::copy(sPath, sNewPath);
  return true;
}

bool File::copy(const std::string &sNewPath, const std::string &sNewName) {
  if (exists()) std::filesystem::copy(sPath, sNewPath + "/" + sNewName);
  return true;
}

bool File::move(const std::string &sNewPath) {
  if (exists()) std::filesystem::rename(sPath, sNewPath);
  return true;
}

bool File::move(const std::string &sNewPath, const std::string &sNewName) {
  if (exists()) std::filesystem::rename(sPath, sNewPath + "/" + sNewName);
  return true;
}

bool File::create() {
  std::ofstream file(sPath);
  if (!file.is_open()) {
    return false;
  }

  file.close();
  return true;
}

std::string File::getFileName() const {
  return std::filesystem::path(sPath).filename().string();
}

std::string File::getFileNameWithoutExtension() const {
  return std::filesystem::path(sPath).stem().string();
}

std::string File::getExtension() const {
  return std::filesystem::path(sPath).extension().string();
}

std::string File::getDirectory() const {
  return std::filesystem::path(sPath).parent_path().string();
}

std::string File::getDirectoryWithoutRoot() const {
  return std::filesystem::path(sPath).parent_path().filename().string();
}

std::string File::getRoot() const {
  return std::filesystem::path(sPath).root_path().string();
}

std::string File::getAbsolutePath() const {
  if (exists()) return std::filesystem::absolute(sPath).string();
  return "";
}

std::string File::getRelativePath() const {
  if (exists()) return std::filesystem::relative(sPath).string();
  return "";
}

bool File::exists() const { return std::filesystem::exists(sPath); }

bool File::isFile() const {
  if (exists()) return std::filesystem::is_regular_file(sPath);
  return false;
}

bool File::isDirectory() const {
  if (exists()) return std::filesystem::is_directory(sPath);
  return false;
}

size_t File::Hash() const { return std::hash<std::string>{}(sPath); }

unsigned int File::size() const {
  if (exists()) return std::filesystem::file_size(sPath);
  return 0;
}

unsigned int File::magic_num() const { return iMagicNum; }

void File::set_magic_num() {
  if (exists()) {
    std::ifstream file(sPath, std::ios::binary);
    if (file.is_open()) {
      file.read((char *)&iMagicNum, sizeof(iMagicNum));
      file.close();
    }
  }
}

#pragma endregion

#pragma region Directory

Directory::Directory(std::string sPath) : sPath(std::move(sPath)) {
  getDirectories();
  getFiles();
}

Directory::Directory(File &file) {
  sPath = file.getDirectory();

  getDirectories();
  getFiles();
}

size_t Directory::Hash() const { return std::hash<std::string>{}(sPath); }

std::string Directory::getRelativePath() const {
  if (exists()) return std::filesystem::relative(sPath).string();
  return "";
}

std::string Directory::getAbsolutePath() const {
  if (exists()) return std::filesystem::absolute(sPath).string();
  return "";
}

std::vector<File> Directory::getFiles() {
  if (exists()) {
    for (const auto &entry : std::filesystem::directory_iterator(sPath)) {
      if (entry.is_regular_file()) vFiles.emplace_back(entry.path().string());
    }
  }
  return vFiles;
}

std::vector<Directory> Directory::getDirectories() {
  if (exists()) {
    for (const auto &entry : std::filesystem::directory_iterator(sPath)) {
      if (entry.is_directory())
        vDirectories.emplace_back(entry.path().string());
    }
  }
  return vDirectories;
}

std::vector<File> Directory::getFiles(const std::string &sExtension) {
  if (exists()) {
    for (const auto &entry : std::filesystem::directory_iterator(sPath)) {
      if (entry.is_regular_file() && entry.path().extension() == sExtension)
        vFiles.emplace_back(entry.path().string());
    }
  }
  return vFiles;
}

std::vector<Directory> Directory::getDirectories(
    const std::string &sExtension) {
  if (exists()) {
    for (const auto &entry : std::filesystem::directory_iterator(sPath)) {
      if (entry.is_directory() && entry.path().extension() == sExtension)
        vDirectories.emplace_back(entry.path().string());
    }
  }
  return vDirectories;
}

bool Directory::exists() const { return std::filesystem::exists(sPath); }

bool Directory::create() {
  if (!exists()) return std::filesystem::create_directory(sPath);
  return false;
}

bool Directory::remove() {
  if (exists()) return std::filesystem::remove(sPath);
  return false;
}

bool Directory::rename(const std::string &sNewName) {
  if (exists()) std::filesystem::rename(sPath, sNewName);
  return true;
}

bool Directory::copy(const std::string &sNewPath) {
  if (exists()) std::filesystem::copy(sPath, sNewPath);
  return true;
}

bool Directory::copy(const std::string &sNewPath, const std::string &sNewName) {
  if (exists()) std::filesystem::copy(sPath, sNewPath + "/" + sNewName);
  return true;
}

bool Directory::move(const std::string &sNewPath) {
  if (exists()) std::filesystem::rename(sPath, sNewPath);
  return true;
}

bool Directory::move(const std::string &sNewPath, const std::string &sNewName) {
  if (exists()) std::filesystem::rename(sPath, sNewPath + "/" + sNewName);
  return true;
}

std::string Directory::getDirectoryName() const {
  return std::filesystem::path(sPath).filename().string();
}

std::string Directory::getParentDirectory() const {
  return std::filesystem::path(sPath).parent_path().string();
}

std::string Directory::getDirectoryWithoutRoot() const {
  return std::filesystem::path(sPath).parent_path().filename().string();
}

std::string Directory::getRoot() const {
  return std::filesystem::path(sPath).root_path().string();
}

size_t Directory::getFilesCount() const { return vFiles.size(); }

size_t Directory::getDirectoriesCount() const { return vDirectories.size(); }

std::vector<File> Directory::getAllFiles() const {
  std::vector<File> vFiles;
  if (exists()) {
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator(sPath)) {
      if (entry.is_regular_file()) vFiles.emplace_back(entry.path().string());
    }
  }
  return vFiles;
}

#pragma endregion