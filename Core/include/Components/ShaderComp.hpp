#pragma once
#include <string>

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

  Shader Compile();

 private:
  static std::string ReadSource(const std::string& files);
  static bool CheckCompileStatus(Handle handle);
  Handle CompileShader(unsigned int type, const std::string& src);
};

struct Program {
  Handle handle;
};