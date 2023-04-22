#include <flecs.h>
#include <glad/glad.h>

#include "BaseRenederModule.hpp"
#include "Components/Renderer/RenderComp.hpp"
#include "Marcos/Marcos.hpp"
MOD_BGN(BaseRenderModule)
// void OnRender(entity e, BaseRenderer& render, PerFrameData& data) {
//   // glNamedBufferSubData(data.handle, 0,
//   //                      sizeof(PerFrameData) - sizeof(unsigned int),
//   &data);

//   // glUseProgram(render.handle);
//   // glDrawArraysInstancedBaseInstance(render.drawType /*GL_TRIANGLES*/, 0,
//   3, 1,
//   //                                   0);

//   glEnable(GL_DEPTH_TEST);
//   glDisable(GL_BLEND);

//   glBindVertexArray(render.vao);

//   glUseProgram(render.handle);

// }

void OnBaseRenderer(entity e, BaseRenderer& render, PerFrameData& data) {
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

BaseRenderModule::BaseRenderModule(world& ecs) {
  ecs.module<BaseRenderModule>();

  ecs.system<BaseRenderer, PerFrameData>("OnBaseRenderer")
      .kind(flecs::OnStore)
      .each(OnBaseRenderer);
}

MOD_END(BaseRenderModule)