//
// Created by Thenecromance on 2023/4/22.
//

#include <flecs.h>
#include <glad/glad.h>

#include "FileWatcherModule.hpp"
#include "Logger.hpp"
#include "Phases.hpp"
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

  //  FileWatcherModule::GetInstance()->AddProgram(handle, handles);
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

void AddShaderWatcher(flecs::entity self, ShaderFileWatcher& wathcer) {
  auto ShaderFiles = self.get<ShaderFile>();
  if (!ShaderFiles->vertexShader.empty()) {
    wathcer.Has_vertexShader = true;
    wathcer.vertexShader =
        std::filesystem::last_write_time(ShaderFiles->vertexShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->fragmentShader.empty()) {
    wathcer.Has_fragmentShader = true;
    wathcer.fragmentShader =
        std::filesystem::last_write_time(ShaderFiles->fragmentShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->geometryShader.empty()) {
    wathcer.Has_geometryShader = true;
    wathcer.geometryShader =
        std::filesystem::last_write_time(ShaderFiles->geometryShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->tessellationControlShader.empty()) {
    wathcer.Has_tessellationControlShader = true;
    wathcer.tessellationControlShader =
        std::filesystem::last_write_time(ShaderFiles->tessellationControlShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->tessellationEvaluationShader.empty()) {
    wathcer.Has_tessellationEvaluationShader = true;
    wathcer.tessellationEvaluationShader =
        std::filesystem::last_write_time(
            ShaderFiles->tessellationEvaluationShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->computeShader.empty()) {
    wathcer.Has_computeShader = true;
    wathcer.tessellationControlShader =
        std::filesystem::last_write_time(ShaderFiles->computeShader)
            .time_since_epoch()
            .count();
  }
}

void ShaderFileWatcherOnUpdate(flecs::entity self, ShaderFileWatcher& watcher,
                               ShaderFile& files, Shader& shader) {
  auto GetFileLastTimeWrite = [](const std::string& file) {
    return std::filesystem::last_write_time(file).time_since_epoch().count();
  };
  // clang-format off
  bool NeedCompile = false;
  if ((watcher.Has_vertexShader &&
       watcher.vertexShader != GetFileLastTimeWrite(files.vertexShader))) {
    watcher.vertexShader = GetFileLastTimeWrite(files.vertexShader);
    NeedCompile = true;
  }

  if ((watcher.Has_fragmentShader &&
       watcher.fragmentShader != GetFileLastTimeWrite(files.fragmentShader))) {
    watcher.fragmentShader = GetFileLastTimeWrite(files.fragmentShader);
    NeedCompile = true;
  }
  if ((watcher.Has_geometryShader &&
       watcher.geometryShader != GetFileLastTimeWrite(files.geometryShader))) {
    watcher.geometryShader = GetFileLastTimeWrite(files.geometryShader);
    NeedCompile = true;
  }
  if ((watcher.Has_tessellationControlShader &&
       watcher.tessellationControlShader !=
           GetFileLastTimeWrite(files.tessellationControlShader))) {
    watcher.tessellationControlShader =
        GetFileLastTimeWrite(files.tessellationControlShader);
    NeedCompile = true;
  }
  if ((watcher.Has_tessellationEvaluationShader &&
       watcher.tessellationEvaluationShader !=
           GetFileLastTimeWrite(files.tessellationEvaluationShader))) {
    watcher.tessellationEvaluationShader =
        GetFileLastTimeWrite(files.tessellationEvaluationShader);
    NeedCompile = true;
  }
  if ((watcher.Has_computeShader &&
       watcher.computeShader != GetFileLastTimeWrite(files.computeShader))) {
    watcher.computeShader = GetFileLastTimeWrite(files.computeShader);
    NeedCompile = true;
  };
  // clang-format on

  if (NeedCompile)
    // so far in this method, once the shader file is changed,
    //  all of the other shader file will also be recompiled,that's not what I
    //  want such as(I changed fragment shader, but the vertex shader will also
    //  be recompiled)
    self.set<Shader>(files.Compile(shader));
}
ShaderHotReloadModule::ShaderHotReloadModule(world& ecs) {
  ecs.import <ShaderHotReloadModule>();
  LoadComponent(ecs);
  // hook the ShaderFile and
  ecs.observer<ShaderFile>()
      //      .event(flecs::OnAdd) // should not use this
      .event(flecs::OnSet)
      .each([](flecs::entity self, ShaderFile& files) {
        self.add<ShaderFileWatcher>();
      });

  ecs.observer<ShaderFile>()
      .event(flecs::OnRemove)
      .each([](flecs::entity self, ShaderFile& files) {
        //        self.add<ShaderFileWatcher>();
        self.remove<ShaderFileWatcher>();
      });
  ecs.observer<ShaderFileWatcher>("ShaderWatcher")
      .event(flecs::OnAdd)
      .each(AddShaderWatcher);

  ecs.system<ShaderFileWatcher, ShaderFile, Shader>("ShaderHotReloadSystem")
      .kind(Phase::PreFrame)
      .each(ShaderFileWatcherOnUpdate);
}

void ShaderHotReloadModule::LoadComponent(world& ecs) {
  ecs.component<ShaderFileWatcher>()
      .member<bool>("Has_vertexShader")
      .member<bool>("Has_fragmentShader")
      .member<bool>("Has_geometryShader")
      .member<bool>("Has_tessellationControlShader")
      .member<bool>("Has_tessellationEvaluationShader")
      .member<bool>("Has_computeShader")

      .member<long long>("vertexShader")
      .member<long long>("fragmentShader")
      .member<long long>("geometryShader")
      .member<long long>("tessellationControlShader")
      .member<long long>("tessellationEvaluationShader")
      .member<long long>("computeShader");
}
MOD_END(ProgramModule)