#pragma once

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(BaseRenderModule)
class BaseRender {
 public:
  BaseRender(world& ecs);
};

class FrameBufferModule {
 public:
  FrameBufferModule(world& ecs);
};
MOD_END(BaseRenderModule)