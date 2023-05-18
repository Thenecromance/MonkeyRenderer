#pragma once
#include <flecs.h>
#include "Job.hpp"
#include "Marcos/Marcos.hpp"


// due to redesign of the light system , this module is no longer available
//MOD_BGN(ShadowMapModule)
//
//class ShadowMapModule {
// public:
//  ShadowMapModule(world& ecs);
//
//
//
//
//  private:
//  void LoadShadowMapComponent();
//  void LoadShadowMapSystem();
// private:
//  //   bool isPreFabCreated_ = false;
//  //   flecs::entity prefab_;
//  flecs::world* pWorld_;
//  flecs::entity shadowMapEntity_{};
//};
//
//MOD_END(ShadowMapModule)

MOD_BGN(ShadowMapping)
class ShadowMapModule{
 public:
  explicit ShadowMapModule(world& ecs);
  void LoadSystem();
 private:
  flecs::world* pWorld_{} ;
  
};
MOD_END(ShadowMapping)