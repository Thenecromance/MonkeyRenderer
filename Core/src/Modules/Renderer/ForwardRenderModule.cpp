#include "ForwardRenderModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include "Model/BaseModel.hpp"

MOD_BGN(ForwardRenderModule)
void OnRender(ForwardRenderer& renderer) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glUseProgram(renderer.handle);
  // upload light data
  // upload camera data
  // upload model data
  // upload material data
  // upload shadow data
  // upload other data

  // glBindVertexArray(renderer.vao);
  // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, /*Materials size */, /*Materials
  // handle */); glBindBufferBase(GL_SHADER_STORAGE_BUFFER, /*model matrices
  // size */, /*Model Matrices handle */); glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
  // /*Indirect command*/); glBindBuffer(GL_PARAMETER_BUFFER, /*Indirect
  // command*/); glMultiDrawElementsIndirect(/*RenderType */GL_TRIANGLES,
  // GL_UNSIGNED_INT,( const void*)sizeof(GLsizei), /*shape size*/, 0);

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}



ForwardRender::ForwardRender(world& ecs) {
  ecs.module<ForwardRender>();

  ecs.system<ForwardRenderer>("ForwardRenderSystem")
      .kind(flecs::OnStore)
      .each(OnRender);
}
MOD_END(ForwardRenderModule)