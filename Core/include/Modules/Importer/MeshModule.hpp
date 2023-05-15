#pragma once

#include <flecs.h>

#include <vector>

#include "CommonDef.hpp"
#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(MeshModule)
class MeshModule {
 public:
  MeshModule(world& ecs);

  // made the mesh prefab public so that it can be used by other modules
  // as what I wanted to is when other module is loaded, this could change by
  // other module dynamically also I can used by lookup to get this prefab too
  static entity MeshPrefab;

 private:
  flecs::world* pWorld_{};
};

MOD_END(MeshModule)
