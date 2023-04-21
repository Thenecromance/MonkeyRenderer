//
// Created by Thenecromance on 2023/4/17.
//

#include "DefferedRender.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include "Logger.hpp"
#include "OpenGLApp.hpp"
#include "Renderer/renderComp.hpp"

MOD_BGN(DefferedRender)

void OnDefferedRender(DefferedRenderer& renderer, gBuffer& framebuffer) {
  //  // geom render pass
  //  {
  //    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.handle);
  //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //    glUseProgram(renderer.geomHandle);
  //
  //    //    glDrawElementsInstancedBaseInstance()
  //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //  }
  //  // light pass
  //  {
  //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //    glUseProgram(renderer.lightHandle);
  //    glBindTextures(0, 1, &framebuffer.colorHandle);
  //  }
}

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

void FrameBufferInit(flecs::iter& it, size_t i, FrameBuffer& buffer) {
  glCreateFramebuffers(1, &buffer.handle);
  glBindFramebuffer(GL_FRAMEBUFFER, buffer.handle);

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
  assert(status == GL_FRAMEBUFFER_COMPLETE);
}

void gBufferInit(flecs::iter& it, size_t i, gBuffer& g_buffer) {
  glCreateFramebuffers(1, &g_buffer.handle);
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.handle);

  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);
  {
    g_buffer.positionHandle =
        genTexture(GL_TEXTURE_2D, width, height, GL_RGBA8);
    glTextureParameteri(g_buffer.positionHandle, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTextureParameteri(g_buffer.positionHandle, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glNamedFramebufferTexture(g_buffer.handle, GL_COLOR_ATTACHMENT0,
                              g_buffer.positionHandle, 0);
  }
  {
    g_buffer.NormalHandle = genTexture(GL_TEXTURE_2D, width, height, GL_RGBA8);
    glTextureParameteri(g_buffer.NormalHandle, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTextureParameteri(g_buffer.NormalHandle, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glNamedFramebufferTexture(g_buffer.handle, GL_COLOR_ATTACHMENT1,
                              g_buffer.NormalHandle, 0);
  }
  {
    g_buffer.AlbedoHandle = genTexture(GL_TEXTURE_2D, width, height, GL_RGBA8);
    glTextureParameteri(g_buffer.AlbedoHandle, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTextureParameteri(g_buffer.AlbedoHandle, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glNamedFramebufferTexture(g_buffer.handle, GL_COLOR_ATTACHMENT2,
                              g_buffer.AlbedoHandle, 0);
  }

  const GLenum status =
      glCheckNamedFramebufferStatus(g_buffer.handle, GL_FRAMEBUFFER);
  assert(status == GL_FRAMEBUFFER_COMPLETE);
}

void FrameBufferRelease(flecs::iter& it, size_t i, FrameBuffer& buffer) {
  glDeleteFramebuffers(1, &buffer.handle);
  glDeleteTextures(1, &buffer.colorHandle);
  glDeleteTextures(1, &buffer.depthHandle);
}

DefferedRender::DefferedRender(world& ecs) {
  ecs.module<DefferedRender>();

  ecs.system<DefferedRenderer, gBuffer>("OnDefferedRenderSystem")
      .kind(flecs::OnStore)
      .each(OnDefferedRender);

  // create observer to init frame buffer
  {
    ecs.observer<FrameBuffer>("FrameBufferInitialize")
        .event(flecs::OnAdd)
        .each(FrameBufferInit);
    ecs.observer<FrameBuffer>("FrameBufferRelease")
        .event(flecs::UnSet)
        .each(FrameBufferRelease);
  }

  { ecs.observer<gBuffer>().event(flecs::OnAdd).each(gBufferInit); }
}

MOD_END(DefferedRender)