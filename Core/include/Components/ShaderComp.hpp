#pragma once
#include <array>
#include <string>
#include <vector>

#include "CommonDef.hpp"

struct Shader {
  Handle vertexHandle{0};
  Handle fragmentHandle{0};
  Handle geometryHandle{0};
  Handle tessellationControlHandle{0};
  Handle tessellationEvaluationHandle{0};
  Handle computeHandle{0};
};

struct ShaderFile {
  std::string vertexShader;
  std::string fragmentShader;
  std::string geometryShader;
  std::string tessellationControlShader;
  std::string tessellationEvaluationShader;
  std::string computeShader;
  std::array<std::string, 10> includePaths;

  Shader Compile();
  Shader Compile(Shader& old);

 private:
  static bool CheckCompileStatus(Handle handle);

  // private:
  std::string ReadSource(const std::string& file_path);

  static Handle CompileShader(unsigned int&& type, const std::string& src,
                              Handle handle = 0);
};

// clang-format off
struct ShaderFileWatcher {
  bool Has_vertexShader                         {false};
  bool Has_fragmentShader                       {false};
  bool Has_geometryShader                       {false};
  bool Has_tessellationControlShader            {false};
  bool Has_tessellationEvaluationShader         {false};
  bool Has_computeShader                        {false};
  std::array<bool, 10> Has_includePaths         {false};
  long long vertexShader                        {0};
  long long fragmentShader                      {0};
  long long geometryShader                      {0};
  long long tessellationControlShader           {0};
  long long tessellationEvaluationShader        {0};
  long long computeShader                       {0};
  std::array<long long, 10> includePaths        {0};
};
// clang-format on

struct Program {
  Handle handle;
};
