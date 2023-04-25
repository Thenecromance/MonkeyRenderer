//
// Created by Thenecromance on 2023/4/22.
//

#include "ShaderComp.hpp"

#include <glad/glad.h>

#include "File.hpp"
#include "FileWatcherModule.hpp"
#include "Logger.hpp"

Shader ShaderFile::Compile() {
  Shader result;
  if (!vertexShader.empty()) {
    result.vertexHandle =
        CompileShader(GL_VERTEX_SHADER, ReadSource(vertexShader));
    FileWatcherModule::GetInstance()->AddShader(vertexShader,
                                                result.vertexHandle);
  }
  if (!fragmentShader.empty()) {
    result.fragmentHandle =
        CompileShader(GL_FRAGMENT_SHADER, ReadSource(fragmentShader));
    FileWatcherModule::GetInstance()->AddShader(fragmentShader,
                                                result.fragmentHandle);
  }
  if (!geometryShader.empty()) {
    result.geometryHandle =
        CompileShader(GL_GEOMETRY_SHADER, ReadSource(geometryShader));
    FileWatcherModule::GetInstance()->AddShader(geometryShader,
                                                result.geometryHandle);
  }
  if (!tessellationControlShader.empty()) {
    result.tessellationControlHandle = CompileShader(
        GL_TESS_CONTROL_SHADER, ReadSource(tessellationControlShader));
    FileWatcherModule::GetInstance()->AddShader(
        tessellationControlShader, result.tessellationControlHandle);
  }
  if (!tessellationEvaluationShader.empty()) {
    result.tessellationEvaluationHandle = CompileShader(
        GL_TESS_EVALUATION_SHADER, ReadSource(tessellationEvaluationShader));
    FileWatcherModule::GetInstance()->AddShader(
        tessellationEvaluationShader, result.tessellationEvaluationHandle);
  }
  if (!computeShader.empty()) {
    result.computeHandle =
        CompileShader(GL_COMPUTE_SHADER, ReadSource(computeShader));
    FileWatcherModule::GetInstance()->AddShader(computeShader,
                                                result.computeHandle);
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