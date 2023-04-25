//
// Created by Thenecromance on 2023/4/22.
//

#include <flecs.h>
#include <glad/glad.h>

#include "FileWatcherModule.hpp"
#include "Logger.hpp"
#include "ProgramModule.hpp"
#include "ShaderComp.hpp"

MOD_BGN(ProgramModule)
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
void ShaderFileOnRemove(flecs::iter&, size_t, ShaderFile& files) {}

void ShaderOnSet(flecs::iter& it, size_t i, Shader& shader) {
  auto self = it.entity(i);
  int result =
      (shader.vertexHandle == 0) + (shader.fragmentHandle == 0) +
      (shader.geometryHandle == 0) + (shader.tessellationControlHandle == 0) +
      (shader.tessellationEvaluationHandle == 0) + (shader.computeHandle == 0);
  if (result <= 1) {
    return;
  }
  Logger::get<ProgramModule>()->trace("{}", self.name());
  Handle handle = glCreateProgram();
  if (handle == 0) {
    assert(false && "ProgramWorker::Compile: glCreateProgram failed");
  }
  std::vector<Handle> handles;
  if (shader.vertexHandle != 0) {
    glAttachShader(handle, shader.vertexHandle);
    Logger::get<ProgramModule>()->trace("Linked vertexHandle:{}",
                                        shader.vertexHandle);

    handles.push_back(shader.vertexHandle);
  }
  if (shader.fragmentHandle != 0) {
    glAttachShader(handle, shader.fragmentHandle);
    handles.push_back(shader.fragmentHandle);
    Logger::get<ProgramModule>()->trace("Linked fragmentHandle:{}",
                                        shader.fragmentHandle);
  }
  if (shader.geometryHandle != 0) {
    glAttachShader(handle, shader.geometryHandle);
    handles.push_back(shader.geometryHandle);
    Logger::get<ProgramModule>()->trace("Linked geometryHandle:{}",
                                        shader.geometryHandle);
  }
  if (shader.tessellationControlHandle != 0) {
    glAttachShader(handle, shader.tessellationControlHandle);
    handles.push_back(shader.tessellationControlHandle);
    Logger::get<ProgramModule>()->trace("Linked tessellationControlHandle:{}",
                                        shader.tessellationControlHandle);
  }
  if (shader.tessellationEvaluationHandle != 0) {
    glAttachShader(handle, shader.tessellationEvaluationHandle);
    handles.push_back(shader.tessellationEvaluationHandle);
    Logger::get<ProgramModule>()->trace(
        "Linked tessellationEvaluationHandle:{}",
        shader.tessellationEvaluationHandle);
  }
  if (shader.computeHandle != 0) {
    glAttachShader(handle, shader.computeHandle);
    handles.push_back(shader.computeHandle);
    Logger::get<ProgramModule>()->trace("Linked computeHandle:{}",
                                        shader.computeHandle);
  }

  glLinkProgram(handle);
  if (!CheckLinkStatus(handle)) {
    glDeleteProgram(handle);
    Logger::get<ProgramModule>()->error("Program link failed");
    return;
  }
  
  FileWatcherModule::GetInstance()->AddProgram(handle, handles);
  self.set<Program>({handle});
}

void ShaderOnRemove(flecs::iter&, size_t, Shader& shader) {
  if (shader.vertexHandle != 0) glDeleteShader(shader.vertexHandle);
  if (shader.fragmentHandle != 0) glDeleteShader(shader.fragmentHandle);
  if (shader.geometryHandle != 0) glDeleteShader(shader.geometryHandle);
  if (shader.tessellationControlHandle != 0)
    glDeleteShader(shader.tessellationControlHandle);
  if (shader.tessellationEvaluationHandle != 0)
    glDeleteShader(shader.tessellationEvaluationHandle);
  if (shader.computeHandle != 0) glDeleteShader(shader.computeHandle);
}

void ProgramOnRemove(flecs::iter&, size_t, Program& program) {
  glDeleteProgram(program.handle);
}

ProgramModule::ProgramModule(world& ecs) {
  ecs.module<ProgramModule>();

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

MOD_END(ProgramModule)