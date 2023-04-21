#pragma once

#include "Patterns/SingleTon.hpp"
#include "Resources/ResourceLoader.hpp"

class ShaderLoader : public ResourceLoader, public LazySingleTon<ShaderLoader> {
 public:
  ShaderLoader();

  Buffer InsertBufferInfo(const std::string &path,
                          std::vector<char> &buffer) override;

 private:
  unsigned int GetShaderType(const std::string &path);
};

enum ShaderType {
  Vertex = 0,
  Fragment,
  Geometry,
  TessControl,
  TessEvaluation,
  Compute,
  ShaderTypeCount
};

typedef std::array<UID, ShaderType::ShaderTypeCount> Shaders;
typedef std::array<std::string, ShaderType::ShaderTypeCount> ShaderFiles;

class WorkerInterface;

class ProgramLoader : public LazySingleTon<ProgramLoader> {
 public:
  UID LoadAndLink(const Shaders &shaders, const std::string &name = "");

  Handle LoadAndLink(const ShaderFiles &shaderFiles,
                     const std::string &name = "");

  //	UID LoadNotLink(Shaders &shaders, const std::string &name = "");

  Handle GetHandle(UID uid) { return mapProgramHandle[uid]; }

  UID GetUID(const std::string &name) {
    for (auto &it : mapProgramNames) {
      if (it.second == name) {
        return it.first;
      }
    }
    return 0;
  }

  std::string GetResourceName(UID uid) { return mapProgramNames[uid]; }

  bool IsLinked(UID uid);

  bool Erase(UID uid);

  bool CleanUpAll();

 public:
  ProgramLoader();

  ~ProgramLoader() override;

 protected:
  static Buffer BuildBuffer(const Shaders &shaders);

 private:
  std::unordered_map<UID, std::string> mapProgramNames;
  std::unordered_map<UID, Shaders> mapProgramShaders;
  std::unordered_map<UID, Handle> mapProgramHandle;

  WorkerInterface *pWorker{nullptr};
  std::string mName{};
};

// Handle LoadFrom(const std::string &vs_path, const std::string &fs_path, const
// std::string &name = "");