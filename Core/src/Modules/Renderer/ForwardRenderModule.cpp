#include "ForwardRenderModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"
#include "BaseTexture.hpp"
#include "Model/BaseModel.hpp"

using namespace glm;
MOD_BGN(ForwardRenderModule)

using namespace Component;
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

  //  glBindVertexArray(renderer.vao);
  //  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, /*Materials size */,
  //  /*Materials
  //   handle */);
  //  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, /*model matrices
  // size */, /*Model Matrices handle */);
  //  glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
  //               /*Indirect command*/);
  //  glBindBuffer(GL_PARAMETER_BUFFER, /*Indirect
  //   command*/);
  //  glMultiDrawElementsIndirect(/*RenderType */ GL_TRIANGLES, GL_UNSIGNED_INT,
  //                              (const void*)sizeof(GLsizei), /*shape size*/,
  //                              0);

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}

/*
void UpdateTransform(){
  // transform the position to view space
  const mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f));
  const mat4 rot =
      glm::rotate(mat4(1.0f), glm::radians(180.0f), rotation->value);
  //  const glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
  const mat4 pos_ = glm::translate(mat4(1.0f), pos->value);
  const mat4 m = glm::rotate(scale * rot * pos_, 0.1f, vec3(0.0f, 0.0f, 1.0f));
  glNamedBufferSubData(model->hMatrices, 0, sizeof(mat4), value_ptr(m));
}
*/

void UpdateModelInfo(const Model* model) {
  glBindVertexArray(model->hVAO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, model->hVertices);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, model->hMatrices);
}
void OnRenderIter(flecs::iter& it) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  for (auto i : it) {
    auto target = it.entity(i);

    const auto model = target.get<Model>();
    const auto texture = target.get<Texture>();
    const auto position = target.get<Position>();
    const auto rotation = target.get<Rotation>();

    glUseProgram(target.get<ForwardRenderer>()->handle);

    // update Texture
    {
      if (texture) {
        glBindTextures(0, 1, &target.get<Texture>()->hHandle);
      }
    }

    //  upload Mesh info
    {
      if (model) {
        UpdateModelInfo(model);
        glDrawElementsInstancedBaseInstance(
            GL_TRIANGLES,
            static_cast<GLsizei>(target.get<Model>()->uIndicesSize),
            GL_UNSIGNED_INT, nullptr, 1, 0);
      }
    }
  }

  glEnable(GL_BLEND);
}

ForwardRender::ForwardRender(world& ecs) {
  ecs.module<ForwardRender>();

  /*  ecs.system<ForwardRenderer>("ForwardRenderSystem")
        .kind(flecs::OnStore)
        .each(OnRender);*/
  // use iter to draw
  ecs.system<ForwardRenderer>("ForwardRenderSystem")
      .kind(flecs::OnUpdate)
      .iter(OnRenderIter);
}
MOD_END(ForwardRenderModule)