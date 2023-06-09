#include <flecs.h>
#include <glad/glad.h>

#include <OpenGLApp.hpp>

#include "BaseRenederModule.hpp"
#include "Logger.hpp"
#include "Marcos/Marcos.hpp"
#include "RenderComp.hpp"

COMP_BGN(RenderComp)
void FrameBuffer::Bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, handle);
  glViewport(0, 0, width, height);
}
void FrameBuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
COMP_END(RenderComp)

MOD_BGN(BaseRender)
using namespace Component;

void FrameBufferInit(flecs::iter& it, size_t i, FrameBuffer& buffer) {
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
  glCreateFramebuffers(1, &buffer.handle);
  // glBindFramebuffer(GL_FRAMEBUFFER, buffer.handle);

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

void FrameBufferRelease(flecs::iter& it, size_t i, FrameBuffer& buffer) {
  glDeleteFramebuffers(1, &buffer.handle);
  glDeleteTextures(1, &buffer.colorHandle);
  glDeleteTextures(1, &buffer.depthHandle);
}

FrameBufferModule::FrameBufferModule(world& ecs) {
  ecs.module<FrameBufferModule>();

  {
    ecs.observer<FrameBuffer>("FrameBufferOnSet")
        .event(flecs::OnAdd)
        .each(FrameBufferInit);

    ecs.observer<FrameBuffer>("FrameBufferRelease")
        .event(flecs::UnSet)
        .event(flecs::OnRemove)
        .each(FrameBufferRelease);
  }
}

MOD_END(BaseRender)

namespace Monkey::Module {
void OnBaseRenderer(entity e, BaseRenderComp& render) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glUseProgram(render.handle);
  glBindVertexArray(render.vao);

  glDrawArraysInstancedBaseInstance(render.drawType /*GL_TRIANGLES*/, 0, 3, 1,
                                    0);

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}

void RemoveOtherRenderer(entity self, BaseRenderComp& render) {
  if (self.has<ForwardRenderComp>()) self.remove<ForwardRenderComp>();
  if (self.has<DefferedRenderComp>()) self.remove<DefferedRenderComp>();
}

BaseRender::BaseRender(world& ecs) {
  ecs.module<BaseRender>();

  ecs.component<FrameBuffer>()
      .member<Handle>("handle")
      .member<Handle>("colorHandle")
      .member<Handle>("depthHandle");

  ecs.system<BaseRenderComp>("OnBaseRenderer")
      .kind(flecs::OnStore)
      .each(OnBaseRenderer);

  ecs.observer<BaseRenderComp>().event(flecs::OnSet).each(RemoveOtherRenderer);
}

}  // namespace Monkey::Module
