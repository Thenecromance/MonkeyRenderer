//
// Created by Thenecromance on 2023/4/22.
//

#include "ShaderComp.hpp"

#include <glad/glad.h>

#include "File.hpp"
#include "FileWatcherModule.hpp"
#include "Logger.hpp"
#include "ShaderOperator.hpp"

COMP_BGN(Shader)

Shader ShaderFile::Compile() {
  Shader result;
  if (!vertexShader.empty()) {
    result.vertexHandle =
        CompileShader(GL_VERTEX_SHADER, ReadSource(vertexShader));
  }
  if (!fragmentShader.empty()) {
    result.fragmentHandle =
        CompileShader(GL_FRAGMENT_SHADER, ReadSource(fragmentShader));
  }
  if (!geometryShader.empty()) {
    result.geometryHandle =
        CompileShader(GL_GEOMETRY_SHADER, ReadSource(geometryShader));
  }
  if (!tessellationControlShader.empty()) {
    result.tessellationControlHandle = CompileShader(
        GL_TESS_CONTROL_SHADER, ReadSource(tessellationControlShader));
  }
  if (!tessellationEvaluationShader.empty()) {
    result.tessellationEvaluationHandle = CompileShader(
        GL_TESS_EVALUATION_SHADER, ReadSource(tessellationEvaluationShader));
  }
  if (!computeShader.empty()) {
    result.computeHandle =
        CompileShader(GL_COMPUTE_SHADER, ReadSource(computeShader));
  }

  return result;
}

Shader ShaderFile::Compile(Shader& old) {
  if (!vertexShader.empty()) {
    CompileShader(GL_VERTEX_SHADER, ReadSource(vertexShader), old.vertexHandle);
  }
  if (!fragmentShader.empty()) {
    CompileShader(GL_FRAGMENT_SHADER, ReadSource(fragmentShader),
                  old.fragmentHandle);
  }
  if (!geometryShader.empty()) {
    CompileShader(GL_GEOMETRY_SHADER, ReadSource(geometryShader),
                  old.geometryHandle);
  }
  if (!tessellationControlShader.empty()) {
    CompileShader(GL_TESS_CONTROL_SHADER, ReadSource(tessellationControlShader),
                  old.tessellationControlHandle);
  }
  if (!tessellationEvaluationShader.empty()) {
    CompileShader(GL_TESS_EVALUATION_SHADER,
                  ReadSource(tessellationEvaluationShader),
                  old.tessellationEvaluationHandle);
  }
  if (!computeShader.empty()) {
    CompileShader(GL_COMPUTE_SHADER, ReadSource(computeShader),
                  old.computeHandle);
  }

  return old;
}

Handle ShaderFile::CompileShader(unsigned int&& type, const std::string& src,
                                 Handle handle /*= 0*/) {
  if (handle == 0) handle = glCreateShader(type);
  auto data = src.c_str();
  glShaderSource(handle, 1, &data, nullptr);
  glCompileShader(handle);
  if (CheckCompileStatus(handle)) {
    return handle;
  }
  Logger::get<ShaderFile>()->info("{}", src);
  return 0;
}

bool ShaderFile::CheckCompileStatus(Handle handle) {
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    glGetShaderInfoLog(handle, 512, nullptr, infoLog);
    Logger::get<ShaderFile>()->warn("shader compile failed with info:{}",
                                    infoLog);
    return false;
  }
  return true;
}

std::string ShaderFile::ReadSource(const std::string& file_path) {
  ShaderOperator op(file_path);
  return op.ReadSource(includePaths);
}

COMP_END(Shader)