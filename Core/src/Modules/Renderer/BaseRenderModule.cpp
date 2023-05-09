#include <flecs.h>
#include <glad/glad.h>

#include <OpenGLApp.hpp>

#include "BaseRenederModule.hpp"
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
void OnBaseRenderer(entity e, BaseRenderer& render) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glUseProgram(render.handle);
  glBindVertexArray(render.vao);

  // typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)(GLenum
  // mode, GLint first, GLsizei count, GLsizei instancecount, GLuint
  // baseinstance);
  glDrawArraysInstancedBaseInstance(render.drawType /*GL_TRIANGLES*/, 0, 3, 1,
                                    0);

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}

void RemoveOtherRenderer(entity self, BaseRenderer& render) {
  if (self.has<ForwardRenderer>()) self.remove<ForwardRenderer>();
  if (self.has<DefferedRenderer>()) self.remove<DefferedRenderer>();
}

BaseRender::BaseRender(world& ecs) {
  ecs.module<BaseRender>();

  ecs.system<BaseRenderer>("OnBaseRenderer")
      .kind(flecs::OnStore)
      .each(OnBaseRenderer);

  ecs.observer<BaseRenderer>().event(flecs::OnSet).each(RemoveOtherRenderer);
}

}  // namespace Monkey::Module
