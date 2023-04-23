#pragma once

#include "Job.hpp"

class TextureModule {
 public:
  TextureModule(world& ecs);

 private:
  void LoadObserver(world& ecs);
  void RegisterComponent(world& ecs);
};