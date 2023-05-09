#pragma once

#include <map>
#include <string>

#include "CommonDef.hpp"
#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(ProgramModule)
class ProgramModule {
 public:
  explicit ProgramModule(world& ecs);
};

class ShaderHotReloadModule {
 public:
  explicit ShaderHotReloadModule(world& ecs);

 private:
  static void LoadComponent(world& ecs);
};

MOD_END(ProgramModule)