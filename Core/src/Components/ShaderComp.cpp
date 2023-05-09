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

std::string ShaderFile::ReadSource(const std::string& files) {
  File f(files);

  if (!f.exists()) return "";

  std::vector<std::string> content;
  if (f.read(content)) {
    for (auto& line : content) {
      if (line.find(inc_prefix) != std::string::npos) {
        if (line.find(sys_prefix) != std::string::npos) {
          // system include
          auto start_loc = line.find(sys_prefix);
          auto end_loc = line.find(sys_suffix);
          auto file_name = f.getDirectory() + "/" +
                           line.substr(start_loc + 1, end_loc - start_loc - 1);

          for (int i = 0; i < 10; ++i) {
            if (includePaths[i].empty()) {
              includePaths[i] = file_name;
              break;
            }
          }
          if (f.getExtension() == ".vs" || f.getExtension() == ".vert") {
            line = "#define VERTEX_SHADER\n" + ReadInclude(file_name);
          } else if (f.getExtension() == ".fs" || f.getExtension() == ".frag") {
            line = "#define FRAGMENT_SHADER\n" + ReadInclude(file_name);
          } else if (f.getExtension() == ".gs" || f.getExtension() == ".geom") {
            line = "#define GEOMETRY_SHADER\n" + ReadInclude(file_name);
          } else if (f.getExtension() == ".tcs" ||
                     f.getExtension() == ".tesc") {
            line = "#define TESS_CONTROL_SHADER\n" + ReadInclude(file_name);
          } else if (f.getExtension() == ".tes" ||
                     f.getExtension() == ".tese") {
            line = "#define TESS_EVALUATION_SHADER\n" + ReadInclude(file_name);
          } else if (f.getExtension() == ".cs" || f.getExtension() == ".comp") {
            line = "#define COMPUTE_SHADER\n" + ReadInclude(file_name);
          } else
            line = ReadInclude(file_name);
        }
      }
    }

    return CombineToString(content);
  }
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

Handle ShaderFile::CompileShader(unsigned int&& type, const std::string& src,
                                 Handle handle /*= 0*/) {
  if (handle == 0) handle = glCreateShader(type);
  auto data = src.c_str();
  glShaderSource(handle, 1, &data, nullptr);
  glCompileShader(handle);
  if (CheckCompileStatus(handle)) {
    return handle;
  }
  return 0;
}

std::string ShaderFile::ReadInclude(const std::string& files) {
  File f(files);
  if (!f.exists()) return "";
  return ReadSource(files);
}

std::string ShaderFile::CombineToString(std::vector<std::string>& content) {
  std::string result;
  for (auto& line : content) {
    result += line + "\n";
  }
  return result;
}
