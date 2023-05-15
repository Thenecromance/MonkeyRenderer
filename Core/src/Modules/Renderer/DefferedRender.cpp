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
  const auto texture = it.entity(0).get<TextureHandle>();
  auto render = render_[0];

  // geom pass
  render.gbuffer.Bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(render.geomHandle);
  glBindTextures(0, 1, &texture->handle);
  meshes[0].DrawInstance(GL_TRIANGLES, it.count());
  render.gbuffer.Unbind();
}

void GBufferBlit(flecs::entity self, GBuffer& buffer) {


/*  glBindFramebuffer(GL_READ_BUFFER, buffer.handle);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, buffer.width, buffer.height, 0, 0, buffer.width,
                    buffer.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
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

  InitializeDefault();
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
        defaultGbuffer.enable();
        render.geomHandle = defaultGeomPass.get<Program>()->handle;
        //        render.lightHandle = defaultLightPass.get<Program>()->handle;
        render.gbuffer = *defaultGbuffer.get<GBuffer>();
      });
}

void DefferedRender::ImportSystem() {
  PTR_ASSERT(pWorld_);

  pWorld_->system<Mesh, DefferedRenderComp>("GeometryPass")
      .kind(Phase::RenderStage)
      .iter([&](flecs::iter& it, Mesh* meshes, DefferedRenderComp* render_) {
        auto self = it.entity(0);

        const auto render = self.get<DefferedRenderComp>();
        const auto g_buffer = self.get<GBuffer>();

        const auto texture = it.entity(0).get<TextureHandle>();

        // geom pass
        defaultGbuffer.get_ref<GBuffer>()->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(render->geomHandle);
        glBindTextures(0, 1, &texture->handle);

        meshes[0].DrawInstance(GL_TRIANGLES, it.count());
        defaultGbuffer.get_ref<GBuffer>()->Unbind();
      });

  //  // so far have not build the LightPass, of course this issue is I still
  //  can't decide how to let the light module work with a better way
  //  pWorld_->system<GBuffer>("BlitToScreen")
  //      .kind(Phase::BlitRender)
  //      .each(GBufferBlit);

  pWorld_->system<GBuffer>("GBufferDebugUI")
      .kind(Phase::ImGuiRender)
      .each(GBufferUI);
}

void DefferedRender::InitializeDefault() {
  PTR_ASSERT(pWorld_);

  GBuffer buffer;
  GBufferInitialize(buffer);
  defaultGbuffer =
      pWorld_->entity("GloabalGBuffer")
          .set<GBuffer>({buffer});  // here should be trigger the observer
  defaultGbuffer.disable();

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
}

MOD_END(DefferedRender)