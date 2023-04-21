#pragma once

#include "CommonDef.hpp"
#include "Job.hpp"
#include "Marcos/Marcos.hpp"
MOD_BGN(AntiAliasingConfigModule)
struct AntiAliasingConfig {
  bool enableMSAA{false};
  bool enableFXAA{false};

  int msaaLevel{4};
};

// TODO: a AA has not implemented yet just include MSAA
class AntiAliasingConfigModule {
 public:
  explicit AntiAliasingConfigModule(world& ecs);
};
MOD_END(AntiAliasingConfigModule)