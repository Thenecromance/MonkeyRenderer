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
#include "Phases.hpp"
#include "Position.hpp"
#include "Renderer/renderComp.hpp"
#include "ShaderComp.hpp"
#include "Texture.hpp"

COMP_BGN(GBuffer)
void GBuffer::Bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, handle);
  //  glViewport(0, 0, width, height);
}
void GBuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
COMP_END(GBuffer)
MOD_BGN(DefferedRender)

using namespace glm;
using namespace Component;

void OnRenderIter(flecs::iter& it, Mesh* meshes, DefferedRenderComp* render_) {
  auto self = it.entity(0);

  const auto render = self.get<DefferedRenderComp>();
  const auto g_buffer = self.get<GBuffer>();

  const auto texture = it.entity(0).get<TextureHandle>();

  // geom pass
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer->handle);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(render->geomHandle);
  glBindTextures(0, 1, &texture->handle);

  meshes[0].DrawInstance(GL_TRIANGLES,it.count());
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBufferBlit(flecs::entity self, GBuffer& buffer) {
  glBindFramebuffer(GL_READ_BUFFER, buffer.handle);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  /*  glBlitFramebuffer(0, 0, g_buffer->width, g_buffer->height, 0, 0,
                      g_buffer->width, g_buffer->height, GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);*/
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CreateTexture(Handle& handle, int width, int height,
                   int internalformat = GL_RGB16F, unsigned int format = GL_RGB,
                   unsigned int type = GL_FLOAT) {
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);
  glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GBufferInitialize(GBuffer& g_buffer) {
  glCreateFramebuffers(1, &g_buffer.handle);
  glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.handle);
  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);
  // clang-format off
  CreateTexture(g_buffer.positionHandle , width, height   , GL_RGBA8, GL_RGBA,GL_UNSIGNED_BYTE);
  CreateTexture(g_buffer.normalHandle   , width, height   , GL_RGBA8, GL_RGBA,GL_UNSIGNED_BYTE);
  CreateTexture(g_buffer.albedoHandle   , width, height   , GL_RGBA8, GL_RGBA,GL_UNSIGNED_BYTE);
  g_buffer.width = width;
  g_buffer.height = height;

  //glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_buffer.positionHandle , 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_buffer.normalHandle   , 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_buffer.albedoHandle   , 0);

  unsigned int attachments[] = {
                                    GL_COLOR_ATTACHMENT0,
                                    GL_COLOR_ATTACHMENT1,
                                    GL_COLOR_ATTACHMENT2
  };
  glDrawBuffers(3, attachments);
  // clang-format on

  // g buffer
  {
    glCreateRenderbuffers(1, &g_buffer.depthHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, g_buffer.depthHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, g_buffer.depthHandle);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
  const GLenum status =
      glCheckNamedFramebufferStatus(g_buffer.handle, GL_FRAMEBUFFER);
  assert(status == GL_FRAMEBUFFER_COMPLETE);

  Logger::get<DefferedRender>()->info("GBuffer Initialize Handle {}",
                                      g_buffer.handle);
}

void GBufferUI(flecs::entity self, GBuffer& buffer) {
  ImGui::Begin(self.name().c_str());
  ImGui::Text("Position");
  ImGui::Text("Normal");
  ImGui::Text("Albedo");

  ImGui::Image((void*)(intptr_t)buffer.positionHandle, ImVec2(200, 200));
  ImGui::Image((void*)(intptr_t)buffer.normalHandle, ImVec2(200, 200));
  ImGui::Image((void*)(intptr_t)buffer.albedoHandle, ImVec2(200, 200));
  ImGui::Image((void*)(intptr_t)buffer.handle, ImVec2(200, 200));

  ImGui::End();
}

DefferedRender::DefferedRender(flecs::world& ecs) {
  ecs.module<DefferedRender>();
  pWorld_ = &ecs;

  {
    //    defaultLightPass =
    //        pWorld_->entity("DefferedRenderLightPass")
    //            .set<ShaderFile>({"Shaders/DefferedRender/LightPass.vert",
    //                              "Shaders/DefferedRender/LightPass.frag"});
    defaultGeomPass =
        pWorld_->entity("DefferedRenderGeomPass")
            .set<ShaderFile>({"Shaders/DefferedRender/Geom.vert",
                              "Shaders/DefferedRender/Geom.frag"});
  }

  InitializePrefab();
  InitializeObserver();
  InitializeComponent();
  ImportSystem();
}

void DefferedRender::InitializeComponent() {
  PTR_ASSERT(pWorld_);
  pWorld_->component<DefferedRenderComp>()
      .member<Handle>("GeometryBufferHandle")
      .member<Handle>("LightBufferHandle");

  pWorld_->component<GBuffer>()
      .member<Handle>("BufferHandle")
      .member<Handle>("PositionHandle")
      .member<Handle>("NormalHandle")
      .member<Handle>("AlbedoHandle")
      .member<Handle>("DepthHandle");
}

void DefferedRender::InitializeObserver() {
  PTR_ASSERT(pWorld_);

  pWorld_->observer<DefferedRenderComp>("DefferedRenderCompObserver")
      .event(flecs::OnAdd)
      .each([&](flecs::entity self, DefferedRenderComp& render) {
        render.geomHandle = defaultGeomPass.get<Program>()->handle;
        self.is_a(preFab_);
      });
}

void DefferedRender::InitializePrefab() {
  PTR_ASSERT(pWorld_);

  GBuffer buffer;
  GBufferInitialize(buffer);
  defaultGbuffer =
      pWorld_->entity("GloabalGBuffer")
          .set<GBuffer>({buffer});  // here should be trigger the observer

  // clang-format off
  preFab_ = pWorld_->prefab("DefferedRenderPrefab")
                .set<GBuffer>(
                    {
                        buffer
                    });  // use prefab to made each entity
                                              // has the same gbuffer as default
  // clang-format on
}

void DefferedRender::ImportSystem() {
  PTR_ASSERT(pWorld_);

  pWorld_->system<Mesh, DefferedRenderComp>("DefferedRenderOnUpdate")
      .kind(Phase::RenderStage)
      .iter(OnRenderIter);

  // so far don't need to blit to screen
  /*
    pWorld_->system<GBuffer>("BlitToScreen")
        .kind(Phase::BlitRender)
        .iter(GBufferBlit); */

  pWorld_->system<GBuffer>("GBufferDebug")
      .kind(Phase::ImGuiRender)
      .each(GBufferUI);
}

MOD_END(DefferedRender)