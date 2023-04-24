#pragma once

#include <string>

#include "CommonDef.hpp"
#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(ProgramModule)
class ProgramModule {
 public:
  explicit ProgramModule(world& ecs);
};

MOD_END(ProgramModule)