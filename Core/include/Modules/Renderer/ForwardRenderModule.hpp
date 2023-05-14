#pragma once
#include <flecs.h>

#include <string>

#include "Marcos/Marcos.hpp"

MOD_BGN(ForwardRenderModule)

class ForwardRenderModule {
 public:
  explicit ForwardRenderModule(flecs::world& ecs);

 private:
  void CreateDefaultProgram();
  void LoadComponent();


 private:
  flecs::entity prefab_;
  flecs::world* pWorld_;

  //  Handle defaultProgram_;
};
MOD_END(ForwardRenderModule)