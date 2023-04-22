//
// Created by Thenecromance on 2023/4/22.
//

#include "ProgramImporter.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include "Logger.hpp"
#include "ShaderComp.hpp"

bool CheckLinkStatus(Handle handle) {
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

void ShaderFileOnSet(flecs::iter& it, size_t i, ShaderFile& files) {
  auto self = it.entity(i);
  self.set<Shader>(files.Compile());
  self.disable<ShaderFile>();
}
void ShaderFileOnRemove(flecs::iter& it, size_t i, ShaderFile& files) {}

void ShaderOnSet(flecs::iter& it, size_t i, Shader& shader) {
  auto self = it.entity(i);

  int result =
      (shader.vertexHandle == 0) + (shader.fragmentHandle == 0) +
      (shader.geometryHandle == 0) + (shader.tessellationControlHandle == 0) +
      (shader.tessellationEvaluationHandle == 0) + (shader.computeHandle == 0);
  if (result <= 1) {
    return;
  }

  Handle handle = glCreateProgram();
  if (handle == 0) {
    assert(false && "ProgramWorker::Compile: glCreateProgram failed");
  }

#define ATTACH_SHADER(val) \
  if (shader.val != 0) glAttachShader(handle, shader.val);
  ATTACH_SHADER(vertexHandle)
  ATTACH_SHADER(fragmentHandle)
  ATTACH_SHADER(geometryHandle)
  ATTACH_SHADER(tessellationControlHandle)
  ATTACH_SHADER(tessellationEvaluationHandle)
  ATTACH_SHADER(computeHandle)
#undef ATTACH_SHADER
  glLinkProgram(handle);
  if (!CheckLinkStatus(handle)) {
    glDeleteProgram(handle);
    return;
  }
  self.set<Program>({handle});
}

void ShaderOnRemove(flecs::iter& it, size_t i, Shader& shader) {
  if (shader.vertexHandle != 0) glDeleteShader(shader.vertexHandle);
  if (shader.fragmentHandle != 0) glDeleteShader(shader.fragmentHandle);
  if (shader.geometryHandle != 0) glDeleteShader(shader.geometryHandle);
  if (shader.tessellationControlHandle != 0)
    glDeleteShader(shader.tessellationControlHandle);
  if (shader.tessellationEvaluationHandle != 0)
    glDeleteShader(shader.tessellationEvaluationHandle);
  if (shader.computeHandle != 0) glDeleteShader(shader.computeHandle);
}

void ProgramOnRemove(flecs::iter& it, size_t i, Program& program) {
  glDeleteProgram(program.handle);
}

ProgramImporter::ProgramImporter(world& ecs) {
  ecs.module<ProgramImporter>();

  {
    ecs.observer<ShaderFile>().event(flecs::OnSet).each(ShaderFileOnSet);
    ecs.observer<ShaderFile>().event(flecs::OnRemove).each(ShaderFileOnRemove);
  }

  {
    ecs.observer<Shader>().event(flecs::OnSet).each(ShaderOnSet);
    ecs.observer<Shader>().event(flecs::OnRemove).each(ShaderOnRemove);
  }
  { ecs.observer<Program>().event(flecs::OnRemove).each(ProgramOnRemove); }
}
