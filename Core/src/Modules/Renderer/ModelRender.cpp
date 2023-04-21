#include "ModelRender.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include <glm/ext.hpp>

#include "Base/Position.hpp"
#include "BaseModel.hpp"
#include "ImGuiComponentDraw.hpp"
#include "Logger.hpp"
#include "RenderComp.hpp"
#include "Textures/BaseTexture.hpp"

using namespace glm;
using namespace Monkey::Component;

MOD_BGN(ModelRender)

/// @brief
/// @param model
/// @param texture
/// @param pos
void RenderBaseModel(flecs::entity e, Model& model, Texture& texture,
                     Position& pos) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

    if (e.has<PostProcess>() && e.has<FrameBuffer>()) {
      glBindFramebuffer(GL_FRAMEBUFFER, e.get<FrameBuffer>()->handle);
      // make sure we clear the framebuffer's content
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

  // update model position and angle etc
  if (model.hMatrices != 0) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, model.hMatrices);
    const mat4 Matrices[2]{
        glm::rotate(glm::translate(mat4(1.0f), pos.value), 90.0f, vec3(1.0f)),
        glm::scale(mat4(1.0f), vec3(10.0f))};

    glNamedBufferSubData(model.hMatrices, 0, sizeof(mat4) * 2, &Matrices);
  }

  glUseProgram(model.hProgram);
  glBindVertexArray(model.hVAO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, model.hVertices);

  glBindTextures(0, 1, &texture.hHandle);
  glDrawElementsInstancedBaseVertexBaseInstance(
      GL_TRIANGLES, model.uIndicesSize, GL_UNSIGNED_INT, nullptr, 1, 0, 0);

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}
void BaseModelUI(flecs::entity e, Model& model, Texture& texture,
                 Position& pos) {
  Begin(e.name().c_str());
  Draw(model);
  Draw(texture);
  Draw(pos);
  End();
}
ModelRender::ModelRender(world& ecs) {
  ecs.module<ModelRender>();
  Logger::get<ModelRender>()->info("ModelRender Loaded");

  ecs.system<Model, Texture, Position>("RenderBaseModel")
      .kind(flecs::PreStore)
      .each(RenderBaseModel);

  ecs.system<Model, Texture, Position>("Model Render UI")
      .kind(flecs::PreUpdate)
      .each(BaseModelUI);
}

MOD_END(ModelRender)