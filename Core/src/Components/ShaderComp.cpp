//
// Created by Thenecromance on 2023/4/22.
//

#include "ShaderComp.hpp"

#include <glad/glad.h>

#include "File.hpp"
#include "Logger.hpp"

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

std::string ShaderFile::ReadSource(const std::string& files) {
  File f(files);
  if (f.exists()) {
    std::string src;
    if (f.read(src)) {
      return src;
    }
  }
  return "";
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

Handle ShaderFile::CompileShader(unsigned int type, const std::string& src) {
  Handle handle = glCreateShader(type);
  auto data = src.c_str();
  glShaderSource(handle, 1, &data, nullptr);
  glCompileShader(handle);
  if (CheckCompileStatus(handle)) {
    return handle;
  }
  return 0;
}