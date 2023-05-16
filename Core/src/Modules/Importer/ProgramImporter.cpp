//
// Created by Thenecromance on 2023/4/22.
//

#include <flecs.h>
#include <glad/glad.h>

#include <atomic>

#include "FileWatcherModule.hpp"
#include "Logger.hpp"
#include "Phases.hpp"
#include "ProgramModule.hpp"
#include "ShaderComp.hpp"

MOD_BGN(ProgramModule)
using namespace Monkey::Component;  // import Component

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
  //
  //  if(!files.vertexShader.empty()&& exists(files.vertexShader)){
  //      )
  if (files.AllFileExist())
    self.set<Shader>(files.Compile());
  else {
    assert(false && "ShaderFileOnSet: file not exist");
  }
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

  { ecs.component<Program>().member<Handle>("handle"); }
}

void AddIncludePathWatcher(const ShaderFile* ShaderFiles,
                           ShaderFileWatcher& watcher) {
  for (int i = 0; i < 10; i++) {
    watcher.Has_includePaths[i] = !ShaderFiles->includePaths[i].empty();
    if (watcher.Has_includePaths[i])
      watcher.includePaths[i] =
          std::filesystem::last_write_time(ShaderFiles->includePaths[i])
              .time_since_epoch()
              .count();
  }
}

bool IncludePathWatcherUpdate(ShaderFile& files, ShaderFileWatcher& watcher) {
  auto GetFileLastTimeWrite = [](const std::string& file) {
    return std::filesystem::last_write_time(file).time_since_epoch().count();
  };
  for (int i = 0; i < 10; i++) {
    if (watcher.Has_includePaths[i])
      if (GetFileLastTimeWrite(files.includePaths[i]) !=
          watcher.includePaths[i]) {
        watcher.includePaths[i] = GetFileLastTimeWrite(files.includePaths[i]);
        return true;
      }
  }
  return false;
}

void AddShaderWatcher(flecs::entity self, ShaderFileWatcher& watcher) {
  auto ShaderFiles = self.get<ShaderFile>();
  if (!ShaderFiles->vertexShader.empty()) {
    watcher.Has_vertexShader = true;
    watcher.vertexShader =
        std::filesystem::last_write_time(ShaderFiles->vertexShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->fragmentShader.empty()) {
    watcher.Has_fragmentShader = true;
    watcher.fragmentShader =
        std::filesystem::last_write_time(ShaderFiles->fragmentShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->geometryShader.empty()) {
    watcher.Has_geometryShader = true;
    watcher.geometryShader =
        std::filesystem::last_write_time(ShaderFiles->geometryShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->tessellationControlShader.empty()) {
    watcher.Has_tessellationControlShader = true;
    watcher.tessellationControlShader =
        std::filesystem::last_write_time(ShaderFiles->tessellationControlShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->tessellationEvaluationShader.empty()) {
    watcher.Has_tessellationEvaluationShader = true;
    watcher.tessellationEvaluationShader =
        std::filesystem::last_write_time(
            ShaderFiles->tessellationEvaluationShader)
            .time_since_epoch()
            .count();
  }
  if (!ShaderFiles->computeShader.empty()) {
    watcher.Has_computeShader = true;
    watcher.tessellationControlShader =
        std::filesystem::last_write_time(ShaderFiles->computeShader)
            .time_since_epoch()
            .count();
  }

  AddIncludePathWatcher(ShaderFiles, watcher);
}

void ShaderFileWatcherOnUpdate(flecs::entity self, ShaderFileWatcher& watcher,
                               ShaderFile& files, Shader& shader) {
  // TODO: make the hot reload module only reload the changed shader.

  // this method is too useless
  auto GetFileLastTimeWrite = [](const std::string& file) {
    return std::filesystem::last_write_time(file).time_since_epoch().count();
  };

  auto IsInTimeLimit = [&](long long old_time, std::string& file) -> bool {
    // because of the std::filesystem::last_write_time,here will receive 2
    // different time in several ms. so set the limit to 1s
    if (GetFileLastTimeWrite(file) - old_time < 1000) {
      return false;
    }
    return true;
  };

  std::atomic<bool> NeedCompile = false;

  // clang-format on
  if ((watcher.Has_vertexShader &&
       watcher.vertexShader != GetFileLastTimeWrite(files.vertexShader))) {
    if (IsInTimeLimit(watcher.vertexShader, files.vertexShader)) {
      watcher.vertexShader = GetFileLastTimeWrite(files.vertexShader);
      NeedCompile = true;
    }
  }

  if ((watcher.Has_fragmentShader &&
       watcher.fragmentShader != GetFileLastTimeWrite(files.fragmentShader))) {
    if (IsInTimeLimit(watcher.fragmentShader, files.fragmentShader)) {
      watcher.fragmentShader = GetFileLastTimeWrite(files.fragmentShader);
      NeedCompile = true;
    }
  }
  if ((watcher.Has_geometryShader &&
       watcher.geometryShader != GetFileLastTimeWrite(files.geometryShader))) {
    if (IsInTimeLimit(watcher.geometryShader, files.geometryShader)) {
      watcher.geometryShader = GetFileLastTimeWrite(files.geometryShader);
      NeedCompile = true;
    }
  }
  if ((watcher.Has_tessellationControlShader &&
       watcher.tessellationControlShader !=
           GetFileLastTimeWrite(files.tessellationControlShader))) {
    if (IsInTimeLimit(watcher.tessellationControlShader,
                      files.tessellationControlShader)) {
      watcher.tessellationControlShader =
          GetFileLastTimeWrite(files.tessellationControlShader);
      NeedCompile = true;
    }
  }
  if ((watcher.Has_tessellationEvaluationShader &&
       watcher.tessellationEvaluationShader !=
           GetFileLastTimeWrite(files.tessellationEvaluationShader))) {
    if (IsInTimeLimit(watcher.tessellationEvaluationShader,
                      files.tessellationEvaluationShader)) {
      watcher.tessellationEvaluationShader =
          GetFileLastTimeWrite(files.tessellationEvaluationShader);
      NeedCompile = true;
    }
  }
  if ((watcher.Has_computeShader &&
       watcher.computeShader != GetFileLastTimeWrite(files.computeShader))) {
    if (IsInTimeLimit(watcher.computeShader, files.computeShader)) {
      watcher.computeShader = GetFileLastTimeWrite(files.computeShader);
      NeedCompile = true;
    }
  };
  if (IncludePathWatcherUpdate(files, watcher)) {
    NeedCompile = true;
  }

  if (NeedCompile) {
    // so far in this method, once the shader file is changed,
    //  all of the other shader file will also be recompiled,that's not what I
    //  want such as(I changed fragment shader, but the vertex shader will also
    //  be recompiled)
    self.set<Shader>(files.Compile(shader));
  }
}

ShaderHotReloadModule::ShaderHotReloadModule(world& ecs) {
  ecs.import <ShaderHotReloadModule>();
  LoadComponent(ecs);
  // hook the ShaderFile and
  ecs.observer<ShaderFile>()
      //      .event(flecs::OnAdd) // should not use this
      .event(flecs::OnSet)
      .each([](flecs::entity self, ShaderFile& files) {
        if (files.AllFileExist()) self.add<ShaderFileWatcher>();
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