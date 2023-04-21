#include "ShaderWorker.hpp"

#include <glad/glad.h>

#include <cassert>

#include "Buffer/Buffer.hpp"
#include "Logger.hpp"

ROOT_BGN()
Handle ShaderWorker::Compile(std::vector<char> &buffer) {
  Handle handle{0};
  if (buffer.empty()) {
    assert(false && "ShaderWorker::Compile: buffer is empty");
  }

  handle = glCreateShader(GL_VERTEX_SHADER);
  if (handle == 0) {
    assert(false && "ShaderWorker::Compile: glCreateShader failed");
  }

  glShaderSource(handle, 1, (const char **)&buffer[0], nullptr);
  glCompileShader(handle);

  if (!CheckCompileStatus(handle)) {
    assert(false && "ShaderWorker::Compile: glCompileShader failed");
  }

  return handle;
}

Handle ShaderWorker::Compile(Buffer &buffer) {
  Handle handle{0};
  const char *data = buffer.data();
  if (buffer.IsModified()) {
    auto type = buffer.values(0);
    handle = glCreateShader(type);
    Logger::get("ShaderWorker")
        ->debug("Shader type :{} , Handle:{} ", type, handle);
  } else {
    handle = glCreateShader(GL_VERTEX_SHADER);
  }
  if (handle == 0) {
    assert(false && "ShaderWorker::Compile: glCreateShader failed");
  }

  glShaderSource(handle, 1, &data, nullptr);
  glCompileShader(handle);

  if (!CheckCompileStatus(handle)) {
    assert(false && "ShaderWorker::Compile: glCompileShader failed");
  }

  return handle;
}

bool ShaderWorker::Release(Handle handle) {
  // check shader is valid
  if (IsValid(handle)) {
    glDeleteShader(handle);
    return true;
  }
  Logger::get("ShaderWorker")->error("invalid shader handle");
  return false;
}

bool ShaderWorker::IsValid(Handle handle) {
  if (handle == 0)
    return false;  // if handle is 0 ,so don't need to call glIsShader
  return glIsShader(handle);
}

bool ShaderWorker::CheckCompileStatus(Handle handle) {
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    glGetShaderInfoLog(handle, 512, nullptr, infoLog);
    Logger::get("ShaderWorker")
        ->warn("shader compile failed with info:{}", infoLog);
    return false;
  }
  return true;
}

bool ShaderWorker::Reload(Handle handle) {
  assert(false && "ShaderWorker::Reload: not implemented");
  return false;
}

#pragma region ProgramWorker

Handle ProgramWorker::Compile(std::vector<char> &buffer) {
  // change the type of char to unsigned int
  // because the type of GL_VERTEX_SHADER is unsigned int
  Handle handle{0};
  if (buffer.empty()) {
    Logger::get("ProgramWorker")->error("buffer is empty nothing to link");
    //			assert(false && "ProgramWorker::Compile: buffer is
    // empty");
  }

  handle = glCreateProgram();
  if (handle == 0) {
    Logger::get("ProgramWorker")
        ->error("glCreateProgram failed, something goes wrong");
    //			assert(false && "ProgramWorker::Compile: glCreateProgram
    // failed");
  }
  // link shader handle

  for (auto i = 0; i < buffer.size(); i += 4) {
    glAttachShader(handle, *(Handle *)&buffer[i]);
  }
  glLinkProgram(handle);

  if (!CheckLinkStatus(handle)) {
    return 0;
  }
  return handle;
}

Handle ProgramWorker::Compile(Buffer &buffer) {
  Handle handle = glCreateProgram();
  if (handle == 0) {
    assert(false && "ProgramWorker::Compile: glCreateProgram failed");
  }

  if (buffer.IsModified()) {
    // TODO : program maybe have some other options
  }

  // convert buffer to unsigned int
  for (auto i = 0; i < buffer.buffer_size(); i += 4) {
    Handle shaderHandle = static_cast<Handle>(buffer.data()[i]);

    if (shaderHandle != 0) {
      Logger::get("ProgramWorker")
          ->trace("Linking Shader {0} to {1}", shaderHandle, handle);
      glAttachShader(handle, shaderHandle);
    }
  }

  glLinkProgram(handle);
  if (!CheckLinkStatus(handle)) {
    glDeleteProgram(handle);
    return 0;
  }
  return handle;
}

bool ProgramWorker::Release(Handle handle) {
  if (IsValid(handle)) {
    glDeleteProgram(handle);
    return true;
  }
  return false;
}

bool ProgramWorker::Reload(Handle handle) { return false; }

bool ProgramWorker::IsValid(Handle handle) {
  if (handle == 0) return false;
  return glIsProgram(handle);
}

bool ProgramWorker::CheckLinkStatus(Handle handle) {
  // check link program is success or not
  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(handle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(handle, 512, nullptr, infoLog);
    Logger::get("ProgramWorker")
        ->info("Program link failed with info :{}", infoLog);
    return false;
  }
  return true;
}

#pragma endregion

ROOT_END()