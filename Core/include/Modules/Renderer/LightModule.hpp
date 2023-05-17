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
  int poolSize{100};

  unsigned int PointLightBufferGroup{};
  unsigned int DirectionLightBufferGroup{};
  unsigned int SpotLightBufferGroup{};

 private:
  int point_light_count_{};
  int direction_light_count_{};
  int spot_light_count_{};
};

MOD_END(LightModule)