#include "ShadowMapModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include <glm/ext.hpp>

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

ShadowMapModule::ShadowMapModule(world& ecs) {
  ecs.module<ShadowMapModule>();
  pWorld_ = &ecs;
  LoadSystem();
}

void ShadowMapModule::LoadSystem() {
  pWorld_->system<Component::ForwardRenderComp>("ShadowMapSystem")
      .kind(Phase::RenderStage)
      
      ;
}

MOD_END(ShadowMapping)
