//
// Created by Thenecromance on 2023/4/12.
//

#include "PBRModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"
#include "ImGuiComponentDraw.hpp"
#include "Model/BaseModel.hpp"
#include "Textures/PBRTexture.hpp"
using namespace glm;
using namespace Monkey::Component;

MOD_BGN(PBRModule)

void BindPBRTextures(PBRTexture& textures) {
  const std::array<unsigned int, 5> textureUnits{
      textures.hTextures[TextureType::AmbientOcclusion],
      textures.hTextures[TextureType::Emissive],
      textures.hTextures[TextureType::Albedo],
      textures.hTextures[TextureType::Metalness],
      textures.hTextures[TextureType::Normal]};
  glBindTextures(0, textureUnits.size(),
                 textureUnits.data());  // set up pbr textures to texture unit 0
}

void PBR_Render(Model& model, PBRTexture& textures, Position& pos,
                Rotation& rotation) {
  NONE_ZERO_CHECK(model.hProgram);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, model.hMatrices);
  // transform the position to view space
  const mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f));
  const mat4 rot =
      glm::rotate(mat4(1.0f), glm::radians(180.0f), rotation.value);
  //  const glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
  const mat4 pos_ = glm::translate(mat4(1.0f), pos.value);
  const mat4 m = glm::rotate(scale * rot * pos_, 0.1f, vec3(0.0f, 0.0f, 1.0f));
  glNamedBufferSubData(model.hMatrices, 0, sizeof(mat4), value_ptr(m));

  BindPBRTextures(textures);
  glBindVertexArray(model.hVAO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, model.hVertices);

  /* glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(model.uIndicesSize),
                  GL_UNSIGNED_INT, nullptr);*/

  glUseProgram(model.hProgram);
  glDrawElementsInstancedBaseInstance(GL_TRIANGLES,
                                      static_cast<GLsizei>(model.uIndicesSize),
                                      GL_UNSIGNED_INT, nullptr, 1, 0);

  glEnable(GL_BLEND);
}
void PBRModelUI(flecs::entity e, Model& model, PBRTexture& textures,
                Position& pos, Rotation& rot) {
  Begin(e.name().c_str());
  Draw(model);
  Draw(textures);
  Draw(rot);
  Draw(pos);
/*  ImGui::InputFloat("X", &pos.value.x, 0.1f,10.0f);
  ImGui::InputFloat("Y", &pos.value.y, 0.1f,10.0f);
  ImGui::InputFloat("Z", &pos.value.z, 0.1f,10.0f);*/
  End();
}

void PBRBindingSystem(PBRTexture& textures) {}

PBRModule::PBRModule(world& ecs) {
  ecs.module<PBRModule>();
  ecs.system<Model, PBRTexture, Position, Rotation>("PBR_Render")
      .kind(flecs::OnStore)
      .each(PBR_Render);

  ecs.system<Model, PBRTexture, Position, Rotation>()
      .kind(flecs::PostUpdate)
      .each(PBRModelUI);
}
MOD_END(PBRModule)