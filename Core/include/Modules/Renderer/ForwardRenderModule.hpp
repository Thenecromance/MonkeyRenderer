#pragma once
#include <flecs.h>

#include <string>

#include "Marcos/Marcos.hpp"

MOD_BGN(ForwardRenderModule)

/*class ForwardRender {
 public:
  explicit ForwardRender(world& ecs);

 private:
  void BuildForwardPreFab(world& ecs);
  void LoadForwardSystem(world& ecs);
  void LoadObserver(world& ecs);
  void RegisterComponent(world& ecs);

 public:
  static std::string preFab;
};*/

class ForwardRenderModule {
 public:
  explicit ForwardRenderModule(flecs::world& ecs);

 private:
  void CreateDefaultProgram();
  void LoadComponent();

  void OnRenderUpdate(flecs::iter& it, size_t i);

 private:
  bool isPreFabCreated_ = false;
  flecs::entity prefab_;
  flecs::world* pWorld_;

  //  Handle defaultProgram_;
};
MOD_END(ForwardRenderModule)