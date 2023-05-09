#pragma once

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(ShadowMapModule)

class ShadowMapModule {
 public:
  ShadowMapModule(world& ecs);
  



  private:
  void RegisterComponents();

  void LoadObserver();

  void LoadSystem();
 private:
  //   bool isPreFabCreated_ = false;
  //   flecs::entity prefab_;
  flecs::world* pWorld_;
};

MOD_END(ShadowMapModule)