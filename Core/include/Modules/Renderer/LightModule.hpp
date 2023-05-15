#pragma once
#include <flecs.h>

#include "Marcos/Marcos.hpp"

MOD_BGN(LightModule)

class LightModule {
 public:
  explicit LightModule(flecs::world& ecs);
  void InitializeComponent();
  void InitializeObserver();
  void InitializeSystem();
  
  
 private:
  void CreateLightBuffers();
 private:
  flecs::world* pWorld_{};
};

MOD_END(LightModule)