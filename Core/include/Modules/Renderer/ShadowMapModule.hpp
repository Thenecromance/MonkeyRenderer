#pragma once
#include <flecs.h>

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(ShadowMapping)
class ShadowMapModule {
 public:
  explicit ShadowMapModule(world& ecs);

 protected:
  void HookOtherComponents();
  void InitializeShadowMapComponent();
  void InitializeShadowMapSystem();
  void LoadShadowMapShader();


  protected:
  void InitializeLightTransformComponent();
 private:
  flecs::world* pWorld_{};
  flecs::entity shadowMapEntity_{};
  flecs::entity shadowRenderPass_{};
  unsigned int PointLightGroup{};
};

MOD_END(ShadowMapping)