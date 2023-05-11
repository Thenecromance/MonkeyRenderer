#pragma once
#include <flecs.h>

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(TransformModule)
class TransformModule {
 public:
  TransformModule(world& ecs);

 private:
  void LoadTransformGroup();
  void LoadTransformComponent();

 private:
  entity prefab_;
  world* pWorld_{};

  // to avoid all transform update to the same buffer, we need to limit the size
  // once the size is reached, we will create a new buffer
  int transformGroupMaxSize{};
};

MOD_END(TransformModule)