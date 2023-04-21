#pragma once

#include <string>
#include <unordered_map>

#include "CommonDef.hpp"
#include "FileSystem/File.hpp"

class WorkerInterface;

class Buffer;

class ResourceLoader {
 public:
  virtual void SetWorker(WorkerInterface *worker) { pWorker = worker; }

  [[nodiscard]] virtual const std::string &name() const { return mName; }

  virtual void name(const std::string &name) { mName = name; }

 public:
  UID LoadAndCompile(const std::string &path, const std::string &name = "");

  UID LoadNotCompile(const std::string &path, const std::string &name = "");

  Handle GetHandle(UID uid);

  UID GetUID(const std::string &name);

  static UID GetUID(const File &file);

  std::string GetResourceName(UID uid);

  bool IsCompiled(UID uid);

  bool Erase(UID uid);

  bool CleanUpAll();

 private:
  bool EraseByUID(UID uid);

  bool EraseByName(const std::string &name);

  bool EraseByPath(const std::string &path);

  virtual Buffer InsertBufferInfo(const std::string &path,
                                  std::vector<char> &buffer);

 protected:
  std::unordered_map<UID, std::string> mapUIDName;
  std::unordered_map<UID, File> mapPath;
  std::unordered_map<UID, Handle> mapUIDMapper;

  WorkerInterface *pWorker{nullptr};

 protected:
  std::string mName{"ResourceLoader"};
};