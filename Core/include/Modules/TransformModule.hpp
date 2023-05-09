#pragma once
#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(TransformModule)
class TransformModule {
 public:
  TransformModule(world& ecs);

 private:
  world* pWorld_{};
};

MOD_END(TransformModule)