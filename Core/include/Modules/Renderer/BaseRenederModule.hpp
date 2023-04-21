#pragma once

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(BaseRenderModule)
class BaseRenderModule {
 public:
  BaseRenderModule(world& ecs);
};
MOD_END(BaseRenderModule)