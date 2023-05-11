#pragma once

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(ShadowMapModule)

class ShadowMapModule {
 public:
  ShadowMapModule(world& ecs);
  



  private:
  void LoadShadowMapComponent();
  void LoadShadowMapSystem();
 private:
  //   bool isPreFabCreated_ = false;
  //   flecs::entity prefab_;
  flecs::world* pWorld_;
};

MOD_END(ShadowMapModule)