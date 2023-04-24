#pragma once

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(TextureModule)
class TextureModule {
 public:
  TextureModule(world& ecs);

 private:
  static void LoadObserver(world& ecs);
  static void RegisterComponent(world& ecs);
};

MOD_END(TextureModule)