#pragma once

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(BaseRenderModule)
class BaseRender {
 public:
  BaseRender(world& ecs);
};
MOD_END(BaseRenderModule)