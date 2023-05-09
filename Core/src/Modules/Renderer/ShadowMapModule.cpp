#include "ShadowMapModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include "OpenGLApp.hpp"
#include "RenderComp.hpp"

COMP_BGN(RenderComp)
void ShadowMap::Bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, handle);
  glViewport(0, 0, width, height);
}
void ShadowMap::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void SSAO::Bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, handle);
  glViewport(0, 0, width, height);
}
void SSAO::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

COMP_END(RenderComp)

MOD_BGN(ShadowMapModule)

auto genTexture = [](GLenum type, int width, int height,
                     unsigned int internalFormat) -> Handle {
  Handle handle = 0;
  glCreateTextures(type, 1, &handle);
  glTextureParameteri(handle, GL_TEXTURE_MAX_LEVEL, 0);
  glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureStorage2D(handle, 1, internalFormat, width, height);
  return handle;
};

void ShadowMapInit(flecs::entity self, Component::ShadowMap& buffer) {
  glCreateFramebuffers(1, &buffer.handle);

  int colorFormat = GL_RGBA8;
  int depthFormat = GL_DEPTH_COMPONENT24;
  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);

  // Color map
  if (colorFormat) {
    buffer.colorHandle = genTexture(GL_TEXTURE_2D, width, height, colorFormat);
    glTextureParameteri(buffer.colorHandle, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
    glTextureParameteri(buffer.colorHandle, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(buffer.handle, GL_COLOR_ATTACHMENT0,
                              buffer.colorHandle, 0);
  }
  // Depth map
  if (depthFormat) {
    buffer.depthHandle = genTexture(GL_TEXTURE_2D, width, height, depthFormat);
    glTextureParameteri(buffer.depthHandle, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
    glTextureParameteri(buffer.depthHandle, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(buffer.handle, GL_DEPTH_ATTACHMENT,
                              buffer.depthHandle, 0);
  }

  const GLenum status =
      glCheckNamedFramebufferStatus(buffer.handle, GL_FRAMEBUFFER);

  {
    buffer.width = width;
    buffer.height = height;
  }
  assert(status == GL_FRAMEBUFFER_COMPLETE);
}

void ShadowMapRelease(flecs::entity self, Component::ShadowMap& buffer) {
  glDeleteTextures(1, &buffer.colorHandle);
  glDeleteTextures(1, &buffer.depthHandle);
  glDeleteFramebuffers(1, &buffer.handle);
}

void SSAOInit(flecs::entity self, Component::SSAO& buffer) {
  glCreateFramebuffers(1, &buffer.handle);

  int colorFormat = GL_RGBA8;
  int depthFormat = GL_DEPTH_COMPONENT24;
  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);

  // Color map
  if (colorFormat) {
    buffer.colorHandle = genTexture(GL_TEXTURE_2D, width, height, colorFormat);
    glTextureParameteri(buffer.colorHandle, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
    glTextureParameteri(buffer.colorHandle, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(buffer.handle, GL_COLOR_ATTACHMENT0,
                              buffer.colorHandle, 0);
  }
  // Depth map
  if (depthFormat) {
    buffer.depthHandle = genTexture(GL_TEXTURE_2D, width, height, depthFormat);
    glTextureParameteri(buffer.depthHandle, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
    glTextureParameteri(buffer.depthHandle, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(buffer.handle, GL_DEPTH_ATTACHMENT,
                              buffer.depthHandle, 0);
  }

  const GLenum status =
      glCheckNamedFramebufferStatus(buffer.handle, GL_FRAMEBUFFER);

  {
    buffer.width = width;
    buffer.height = height;
  }
  assert(status == GL_FRAMEBUFFER_COMPLETE);
}

void SSAORelease(flecs::entity self, Component::SSAO& buffer) {
  glDeleteTextures(1, &buffer.colorHandle);
  glDeleteTextures(1, &buffer.depthHandle);
  glDeleteFramebuffers(1, &buffer.handle);
}

ShadowMapModule::ShadowMapModule(world& ecs) {
  ecs.module<ShadowMapModule>();
  pWorld_ = &ecs;

  RegisterComponents();
}

void ShadowMapModule::RegisterComponents() {
  pWorld_->component<Component::ShadowMap>("ShadowMap")
      .member<Handle>("handle")
      .member<Handle>("color handle")
      .member<Handle>("depth handle")
      .member<int>("height")
      .member<int>("width");

  pWorld_->component<Component::SSAO>("SSAO")
      .member<Handle>("handle")
      .member<Handle>("color handle")
      .member<Handle>("depth handle")
      .member<int>("height")
      .member<int>("width");
}
void ShadowMapModule::LoadObserver() {
  pWorld_->observer<Component::ShadowMap>("ShadowMapCreator")
      .event(flecs::OnAdd)
      .each(ShadowMapInit);
  pWorld_->observer<Component::ShadowMap>("ShadowMapRelease")
      .event(flecs::OnRemove)
      .each(ShadowMapRelease);

  pWorld_->observer<Component::SSAO>("SSAOCreator")
      .event(flecs::OnAdd)
      .each(SSAOInit);
  pWorld_->observer<Component::SSAO>("SSAORelease")
      .event(flecs::OnRemove)
      .each(SSAORelease);
}
void ShadowMapModule::LoadSystem() {
//  pWorld_->system<Component::ShadowMap>("ShadowMapUpdate").each([]() {
//  });                                                            // TODO
//  pWorld_->system<Component::SSAO>("SSAOUpdate").each([]() {});  // TODO
}
MOD_END(ShadowMapModule)
