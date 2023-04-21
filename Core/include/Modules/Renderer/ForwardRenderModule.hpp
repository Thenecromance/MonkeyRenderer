#pragma once

#include "Job.hpp"

#include "Marcos/Marcos.hpp"
#include "Components/Renderer/RenderComp.hpp"
MOD_BGN(ForwardRenderModule)
class ForwardRender {
 public:
  ForwardRender(world& ecs);

};

MOD_END(ForwardRenderModule)