//
// Created by Thenecromance on 2023/4/28.
//

#include "ShadowMap.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "OpenGLApp.hpp"
#include "RenderComp.hpp"
#include "ShaderComp.hpp"

MOD_BGN(ShadowMapModule)
using namespace Component;

void CleanUpFrameBuffer(Handle& handle) {
  glClearNamedFramebufferfv(handle, GL_COLOR, 0,
                            glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
  glClearNamedFramebufferfi(handle, GL_DEPTH_STENCIL, 0, 1.0f, 0);
}
void ShadowMapUpdate(flecs::entity self, FrameBuffer& frameBuffer,
                     const FrameBuffer& perframedata) {
  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.handle);
  CleanUpFrameBuffer(frameBuffer.handle);
  glViewport(0, 0, width, height);

  auto shadowProgram = self.get<Program>();
  if (shadowProgram) glUseProgram(shadowProgram->handle);

  {
    // draw mesh
    /*
     for(auto mesh :meshes){
        glBindVertexArray(mesh.vao);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh.Vertices);
        glDrawElements(GL_TRIANGLES,
        static_cast<GLsizei>(mesh.numIndices),GL_UNSIGNED_INT, nullptr);
     }
     */
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
ShadowMapModule::ShadowMapModule(world& ecs) {
  ecs.module<ShadowMapModule>();

 /* ecs.system<ShadowMapComp, const FrameBuffer>("ShadowMapUpdated")
      .each(ShadowMapUpdate);*/
}

MOD_END(ShadowMapModule)