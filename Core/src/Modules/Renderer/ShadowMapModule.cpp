#include "ShadowMapModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include <glm/ext.hpp>

#include "LightComp.hpp"
#include "OpenGLApp.hpp"
#include "Phases.hpp"
#include "RenderComp.hpp"
#include "ShaderComp.hpp"
COMP_BGN(RenderComp)
void ShadowMap::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, handle);
  glViewport(0, 0, width, height);
}
void ShadowMap::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void SSAO::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, handle);
  glViewport(0, 0, width, height);
}
void SSAO::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

COMP_END(RenderComp)

MOD_BGN(ShadowMapping)

void UpdatePointLightMatrices(flecs::iter& it, Component::PointLight* light,
                              Component::LightTransform* transform) {
  //  const glm::mat4 rotY = glm::rotate(mat4(1.f), g_LightYAngle, glm::vec3(0,
  //  1, 0)); const glm::mat4 rotX = glm::rotate(rotY, g_LightXAngle,
  //  glm::vec3(1, 0, 0)); const glm::vec4 lightPos = rotX * glm::vec4(0, 0,
  //  g_LightDist, 1.0f); const mat4 lightProj =
  //  glm::perspective(glm::radians(g_LightAngle), 1.0f, g_LightNear,
  //  g_LightFar);

  /*  transform[row].view = glm::lookAt(glm::vec3(lightPos), vec3(0), vec3(0, 1,
    0)); transform[row].projection = lightProj;*/
}

void UpdateDirectionalLightMatrices(flecs::iter& it,
                                    Component::DirectionalLight* light,
                                    Component::LightTransform* transform) {}
void UpdateSpotLightMatrices(flecs::iter& it, Component::SpotLight* light,
                             Component::LightTransform* transform) {}

ShadowMapModule::ShadowMapModule(world& ecs) {
  ecs.module<ShadowMapModule>();
  pWorld_ = &ecs;
  LoadObserver();
  LoadSystem();
}

void ShadowMapModule::LoadSystem() {
  PTR_ASSERT(pWorld_);
  // now we can update the light matrices
  {
    pWorld_->system<Component::PointLight, Component::LightTransform>()
        .kind(Phase::LightBinding)
        .iter(UpdatePointLightMatrices);

    pWorld_->system<Component::DirectionalLight, Component::LightTransform>()
        .kind(Phase::LightBinding)
        .iter(UpdateDirectionalLightMatrices);

    pWorld_->system<Component::SpotLight, Component::LightTransform>()
        .kind(Phase::LightBinding)
        .iter(UpdateSpotLightMatrices);
  }
}
void ShadowMapModule::LoadObserver() {
  PTR_ASSERT(pWorld_);

  // add LightTransform component to light entities
  {
    pWorld_->observer<Component::PointLight>()
        .event(flecs::OnAdd)
        .each([](flecs::iter& it, size_t i, Component::PointLight& light) {
          if (it.event() == flecs::OnAdd)
            it.entity(i).add<Component::LightTransform>();
        });
    pWorld_->observer<Component::DirectionalLight>()
        .event(flecs::OnAdd)
        .each(
            [](flecs::iter& it, size_t i, Component::DirectionalLight& light) {
              if (it.event() == flecs::OnAdd)
                it.entity(i).add<Component::LightTransform>();
            });
    pWorld_->observer<Component::SpotLight>()
        .event(flecs::OnAdd)
        .each([](flecs::iter& it, size_t i, Component::SpotLight& light) {
          if (it.event() == flecs::OnAdd)
            it.entity(i).add<Component::LightTransform>();
        });
  }
}
MOD_END(ShadowMapping)
