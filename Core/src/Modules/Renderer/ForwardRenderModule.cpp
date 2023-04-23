#include "ForwardRenderModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"
#include "Model/ModelComp.hpp"
#include "Texture.hpp"

using namespace glm;
MOD_BGN(ForwardRenderModule)

using namespace Component;

void UpdateModelInfo(const Model* model, const Position* pos,
                     const Rotation* rotation) {
  glBindVertexArray(model->hVAO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, model->hVertices);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, model->hMatrices);

  const mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f));
  const mat4 rot =
      glm::rotate(mat4(1.0f), glm::radians(180.0f), rotation->value);
  //  const glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
  const mat4 pos_ = glm::translate(mat4(1.0f), pos->value);
  const mat4 m = glm::rotate(scale * rot * pos_, 0.1f, vec3(0.0f, 0.0f, 1.0f));
  glNamedBufferSubData(model->hMatrices, 0, sizeof(mat4), value_ptr(m));
}

void OnRenderIter(flecs::iter& it) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  for (auto i : it) {
    auto target = it.entity(i);

    const auto model = target.get<Model>();
    const auto texture = target.get<TextureHandle>();
    const auto position = target.get<Position>();
    const auto rotation = target.get<Rotation>();

    glUseProgram(target.get<Model>()->hProgram);

    // update Texture
    {
      if (texture) {
        glBindTextures(0, 1, &texture->handle);
      }
    }

    //  upload Mesh info
    {
      if (model) {
        UpdateModelInfo(model, position, rotation);
        glDrawElementsInstancedBaseInstance(
            GL_TRIANGLES,
            static_cast<GLsizei>(target.get<Model>()->uIndicesSize),
            GL_UNSIGNED_INT, nullptr, 1, 0);
      }
    }
  }

  glEnable(GL_BLEND);
}

void RemoveOtherRenderer(entity self, ForwardRenderer& render) {
  if (self.has<BaseRenderer>()) self.remove<BaseRenderer>();
  if (self.has<DefferedRenderer>()) self.remove<DefferedRenderer>();
}

ForwardRender::ForwardRender(world& ecs) {
  ecs.module<ForwardRender>();

  // use iter to draw
  ecs.system<ForwardRenderer>("ForwardRenderSystem")
      .kind(flecs::OnUpdate)
      .iter(OnRenderIter);

  ecs.observer<ForwardRenderer>().event(flecs::OnSet).each(RemoveOtherRenderer);
}
MOD_END(ForwardRenderModule)