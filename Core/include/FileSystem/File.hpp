#pragma once

#include <string>
#include <vector>

class File {
 public:
  File() = default;

  explicit File(std::string sPath);

  ~File() = default;

 public:
  bool read(std::vector<std::string> &vLines);

  bool read(std::string &sContent);

  bool read(std::vector<char> &vContent);

  bool write(const std::vector<std::string> &vLines);

  bool write(const std::string &sContent);

  bool write(const std::vector<char> &vContent);

  bool append(const std::vector<std::string> &vLines);

  bool append(const std::string &sContent);

  bool append(const std::vector<char> &vContent);

  bool remove();

 public:
  bool rename(const std::string &sNewName);

  bool copy(const std::string &sNewPath);

  bool copy(const std::string &sNewPath, const std::string &sNewName);

  bool move(const std::string &sNewPath);

  bool move(const std::string &sNewPath, const std::string &sNewName);

  bool create();

  [[nodiscard]] std::string getFileName() const;

  [[nodiscard]] std::string getFileNameWithoutExtension() const;

  [[nodiscard]] std::string getExtension() const;

  [[nodiscard]] std::string getDirectory() const;

  [[nodiscard]] std::string getDirectoryWithoutRoot() const;

  [[nodiscard]] std::string getRoot() const;

  [[nodiscard]] std::string getAbsolutePath() const;

  [[nodiscard]] std::string getRelativePath() const;

  [[nodiscard]] bool exists() const;

  [[nodiscard]] bool isFile() const;

  [[nodiscard]] bool isDirectory() const;

  [[nodiscard]] size_t Hash() const;

 public:
  [[nodiscard]] unsigned int size() const;

  [[nodiscard]] unsigned int magic_num() const;

 protected:
  void set_magic_num();

 private:
  std::string sPath;
  unsigned int iMagicNum{0};
};

class Directory {
 public:
  explicit Directory(std::string sPath);

  explicit Directory(File &file);

  ~Directory() = default;

 public:
  [[nodiscard]] std::vector<File> getAllFiles() const;

  [[nodiscard]] size_t getDirectoriesCount() const;

  [[nodiscard]] size_t getFilesCount() const;

 public:
  std::vector<File> getFiles();

  std::vector<Directory> getDirectories();

  [[nodiscard]] std::vector<File> getFiles(const std::string &sExtension);

  [[nodiscard]] std::vector<Directory> getDirectories(
      const std::string &sExtension);

  [[nodiscard]] bool exists() const;

  bool create();

  bool remove();

  bool rename(const std::string &sNewName);

  bool copy(const std::string &sNewPath);

  bool copy(const std::string &sNewPath, const std::string &sNewName);

  bool move(const std::string &sNewPath);

  bool move(const std::string &sNewPath, const std::string &sNewName);

  [[nodiscard]] std::string getDirectoryName() const;

  [[nodiscard]] std::string getParentDirectory() const;

  [[nodiscard]] std::string getDirectoryWithoutRoot() const;

  [[nodiscard]] std::string getRoot() const;

  [[nodiscard]] std::string getAbsolutePath() const;

  [[nodiscard]] std::string getRelativePath() const;

  [[nodiscard]] size_t Hash() const;

 private:
  std::string sPath;
  std::vector<File> vFiles;
  std::vector<Directory> vDirectories;
};


