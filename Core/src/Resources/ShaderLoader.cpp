//
// Created by Thenecromance on 2023/3/30.
//
#include "ShaderLoader.hpp"

#include <glad/glad.h>

#include <array>

#include "Buffer/Buffer.hpp"
#include "Logger.hpp"
#include "ShaderWorker.hpp"
#include "SnowFlake.hpp"

unsigned int ShaderLoader::GetShaderType(const std::string &path) {
  if (mName == "ResourceLoader") mName = "ShaderLoader";

  auto ext = File(path).getExtension();

#define MAP(ret_type, _ext1, _ext2) \
  if (ext == _ext1 || ext == _ext2) return ret_type;
  // according to the extension, return the shader type
  MAP(GL_VERTEX_SHADER, ".vert", ".vs");
  MAP(GL_FRAGMENT_SHADER, ".frag", ".fs");
  MAP(GL_GEOMETRY_SHADER, ".geom", ".gs");
  MAP(GL_TESS_CONTROL_SHADER, ".tesc", ".tcs");
  MAP(GL_TESS_EVALUATION_SHADER, ".tese", ".tes");
  MAP(GL_COMPUTE_SHADER, ".comp", ".cs");
#undef MAP

  return 0;
}

ShaderLoader::ShaderLoader() {
  mName = "ShaderLoader";
  pWorker = new Monkey::ShaderWorker();
}

Buffer ShaderLoader::InsertBufferInfo(const std::string &path,
                                      std::vector<char> &buffer) {
  unsigned int shaderType = GetShaderType(path);
  Buffer buffer_(buffer, 1);
  buffer_.set_value(0, static_cast<int>(shaderType));

  return buffer_;
}

ProgramLoader::ProgramLoader() {
  Logger::get("ProgramLoader")->trace("Constructing ProgramLoader");
  mName = "ProgramLoader";
  pWorker = new Monkey::ProgramWorker();
}

UID ProgramLoader::LoadAndLink(const Shaders &shaders,
                               const std::string &name) {
  Buffer buffer = BuildBuffer(shaders);

  Handle handle_ = pWorker->Compile(buffer);

  Logger::get("ProgramLoader")->trace("Name:[{}]-> Handle:{}", name, handle_);

  // TODO: program's hashed UID
  Snowflake snowflake;
  UID uid = snowflake.next_id();

  if (handle_ != 0) {
    mapProgramNames[uid] = name;
    mapProgramShaders[uid] = shaders;
    mapProgramHandle[uid] = handle_;
  }
  return uid;
}

Buffer ProgramLoader::BuildBuffer(const Shaders &shaders) {
  Buffer buffer((char *)shaders.data(), shaders.size() * sizeof(unsigned int));
  return buffer;
}

ProgramLoader::~ProgramLoader() {
  //	Logger::get("ProgramLoader")->trace("Destructing ProgramLoader");
  CleanUpAll();
}

bool ProgramLoader::IsLinked(UID uid) {
  return pWorker->IsValid(mapProgramHandle[uid]);
}

bool ProgramLoader::Erase(UID uid) {
  if (mapProgramHandle.find(uid) != mapProgramHandle.end()) {
    pWorker->Release(mapProgramHandle[uid]);
    mapProgramHandle.erase(uid);
    mapProgramNames.erase(uid);
    mapProgramShaders.erase(uid);
    return true;
  }
  return false;
}

bool ProgramLoader::CleanUpAll() {
  for (auto &it : mapProgramHandle) {
    pWorker->Release(it.second);
  }
  mapProgramHandle.clear();
  mapProgramNames.clear();
  mapProgramShaders.clear();
  return true;
}

Handle ProgramLoader::LoadAndLink(const ShaderFiles &shaderFiles,
                                  const std::string &name) {
  auto GetShaderIndex = [](const std::string &path) -> int {
    auto ext = File(path).getExtension();

#define MAP(ret_type, _ext1, _ext2) \
  if (ext == _ext1 || ext == _ext2) return ret_type;
    // according to the extension, return the shader type
    MAP(ShaderType::Vertex, ".vert", ".vs");
    MAP(ShaderType::Fragment, ".frag", ".fs");
    MAP(ShaderType::Geometry, ".geom", ".gs");
    MAP(ShaderType::TessControl, ".tesc", ".tcs");
    MAP(ShaderType::TessEvaluation, ".tese", ".tes");
    MAP(ShaderType::Compute, ".comp", ".cs");
#undef MAP
    return ShaderType::ShaderTypeCount;
  };

  Shaders shaders{0};
  for (auto &it : shaderFiles) {
    if (it.empty()) continue;
    auto shaderHandle = ShaderLoader::GetInstance()->LoadAndCompile(it);
    auto index = GetShaderIndex(it);
    if (index == ShaderType::ShaderTypeCount) {
      Logger::get("ProgramLoader")->error("Unknown shader type: {}", it);
      return 0;
    }
    shaders[index] = ShaderLoader::GetInstance()->GetHandle(shaderHandle);
  }
  return GetHandle(LoadAndLink(shaders, name));
}