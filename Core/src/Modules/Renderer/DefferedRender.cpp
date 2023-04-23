//
// Created by Thenecromance on 2023/4/17.
//

#include "DefferedRender.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"
#include "Logger.hpp"
#include "MeshComp.hpp"
#include "OpenGLApp.hpp"
#include "Position.hpp"
#include "Renderer/renderComp.hpp"
#include "Texture.hpp"
MOD_BGN(DefferedRender)

using namespace glm;
using namespace Component;

void UpdateModelInfoTemp(const Mesh* model, const Position* pos,
                         const Rotation* rotation) {
  glBindVertexArray(model->vao);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, model->Vertices);
  /*glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, model->hMatrices);

  const mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f));
  const mat4 rot =
      glm::rotate(mat4(1.0f), glm::radians(180.0f), rotation->value);
  //  const glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
  const mat4 pos_ = glm::translate(mat4(1.0f), pos->value);
  const mat4 m = glm::rotate(scale * rot * pos_, 0.1f, vec3(0.0f, 0.0f, 1.0f));
  glNamedBufferSubData(model->hMatrices, 0, sizeof(mat4), value_ptr(m));*/
}

void OnDefferedRender(flecs::iter& it) {
  for (auto i : it) {
    auto self = it.entity(i);
    auto render = self.get<DefferedRenderer>();
    auto g_buffer = self.get<GBuffer>();

    const auto mesh = self.get<Mesh>();
    const auto texture = self.get<TextureHandle>();
    const auto position = self.get<Position>();
    const auto rotation = self.get<Rotation>();

    {
      glBindFramebuffer(GL_FRAMEBUFFER, g_buffer->handle);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glUseProgram(render->geomHandle);

      if (texture) {
        glBindTextures(0, 1, &texture->handle);
      }
      UpdateModelInfoTemp(mesh, position, rotation);
      glBindVertexArray(mesh->vao);

      glDrawElementsInstancedBaseInstance(
          GL_TRIANGLES, static_cast<GLsizei>(mesh->numIndices), GL_UNSIGNED_INT,
          nullptr, 1, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer->handle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the
    // internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my
    // systems, but if it doesn't on yours you'll likely have to write to the
    // depth buffer in another shader stage (or somehow see to match the default
    // framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, 2560, 1440, 0, 0, 2560, 1440, GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
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

void FrameBufferRelease(flecs::iter& it, size_t i, FrameBuffer& buffer) {
  glDeleteFramebuffers(1, &buffer.handle);
  glDeleteTextures(1, &buffer.colorHandle);
  glDeleteTextures(1, &buffer.depthHandle);
}

void gBufferInit(flecs::iter& it, size_t i, GBuffer& g_buffer) {
  glCreateFramebuffers(1, &g_buffer.handle);
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.handle);

  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);

  // normal buffer
  {
    Handle handle = 0;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           handle, 0);

    g_buffer.positionHandle = handle;
  }
  // normal buffer
  {
    Handle handle = 0;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           handle, 0);
    g_buffer.NormalHandle = handle;
  }

  {
    Handle handle = 0;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                           handle, 0);

    g_buffer.AlbedoHandle = handle;
  }
  unsigned int attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments);
  // g buffer
  {
    glCreateRenderbuffers(1, &g_buffer.depthHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, g_buffer.depthHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, g_buffer.depthHandle);
  }
  const GLenum status =
      glCheckNamedFramebufferStatus(g_buffer.handle, GL_FRAMEBUFFER);
  assert(status == GL_FRAMEBUFFER_COMPLETE);
}

void RemoveOtherRenderer(entity self, DefferedRenderer& render) {
  if (self.has<BaseRenderer>()) self.remove<BaseRenderer>();
  if (self.has<ForwardRenderer>()) self.remove<ForwardRenderer>();
}

void DefferedCompInit(entity self, DefferedRenderer& renderer) {
  
  
  RemoveOtherRenderer(self, renderer);
  self.add<GBuffer>();
}

void GBufferUI(GBuffer& buffer) {
  ImGui::Begin("GBuffer");
  ImGui::Text("Position");
  ImGui::SameLine();
  ImGui::Text("Normal");
  ImGui::SameLine();
  ImGui::Text("Albedo");
  ImGui::Image((void*)(intptr_t)buffer.positionHandle, ImVec2(200, 200));
  ImGui::SameLine();
  ImGui::Image((void*)(intptr_t)buffer.NormalHandle, ImVec2(200, 200));
  ImGui::SameLine();
  ImGui::Image((void*)(intptr_t)buffer.AlbedoHandle, ImVec2(200, 200));

  ImGui::End();
}

DefferedRender::DefferedRender(world& ecs) {
  ecs.module<DefferedRender>();

  ecs.system<DefferedRenderer, GBuffer>("OnDefferedRenderSystem")
      .kind(flecs::OnStore)
      .iter(OnDefferedRender);

  ecs.system<GBuffer>("gBufferUI").kind(flecs::PreUpdate).each(GBufferUI);

  // create observer to init frame buffer
  {
    ecs.observer<FrameBuffer>("FrameBufferInitialize")
        .event(flecs::OnAdd)
        .each(FrameBufferInit);
    ecs.observer<FrameBuffer>("FrameBufferRelease")
        .event(flecs::UnSet)
        .event(flecs::OnRemove)
        .each(FrameBufferRelease);
  }

  {
    ecs.observer<DefferedRenderer>()
        .event(flecs::OnAdd)
        .event(flecs::OnSet)
        .each(DefferedCompInit);
    ecs.observer<GBuffer>().event(flecs::OnAdd).each(gBufferInit);
  }
}

MOD_END(DefferedRender)