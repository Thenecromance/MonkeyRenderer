#include "LightModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include <OpenGLApp.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "GlobalValue.hpp"
#include "LightComp.hpp"
#include "MeshComp.hpp"
#include "Phases.hpp"

//// update light buffer
// void OnPointLightUpdate(PointLight& light) {
//   //  if (lightHandle == 0) {
//   //    glCreateBuffers(1, &lightHandle);
//   //    glNamedBufferStorage(lightHandle, sizeof(PointLight), nullptr,
//   //                         GL_DYNAMIC_STORAGE_BIT);
//   //    glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightHandle, 0,
//   //    sizeof(PointLight));
//   //  }
//   //  glNamedBufferSubData(lightHandle, 0, sizeof(PointLight), &light);
// }
//
// void LightSpaceMatricesUpdate(LightTransform& transform){
//
// }
//
//
// void LightUI(PointLight& light) {
//   ImGui::Begin("Light");
//   ImGui::InputFloat("Position X:", &light.position.x, 1.0f, 10.0f);
//   ImGui::InputFloat("Position Y:", &light.position.y, 1.0f, 10.0f);
//   ImGui::InputFloat("Position Z:", &light.position.z, 1.0f, 10.0f);
//
//   ImGui::SliderFloat("Color R:", &light.color.r, 0.0f, 1.0f);
//   ImGui::SliderFloat("Color G:", &light.color.g, 0.0f, 1.0f);
//   ImGui::SliderFloat("Color B:", &light.color.b, 0.0f, 1.0f);
//   ImGui::End();
// }
//
//
// LightModule::LightModule(world& ecs) {
//   ecs.module<LightModule>();
//
//   ecs.observer<PointLight>()
//       .event(flecs::OnAdd)
//       .each([](flecs::iter& it, size_t i, PointLight& light) {
//         light.position = {1.0f, 1.0, 1.0};
//         light.color = {1.0f, 1.0, 1.0};
//         light.intensity = 1.0f;
//       });
//
//   // update Point light first
//   ecs.system<PointLight>("PointLightUpdate")
//       .kind(flecs::PostUpdate)
//       .each(OnPointLightUpdate);
//
//   ecs.system<LightTransform>("LightMatricesUpdate")
//       .kind(Monkey::Phase::LightBinding)
//       .each(LightSpaceMatricesUpdate);
//
//
//   ecs.system<PointLight>("PointLightUI").kind(flecs::PreUpdate).each(LightUI);
// }

MOD_BGN(LightModule)
using namespace Monkey::Component;

// need to find a way to upload light data to gpu

LightModule::LightModule(flecs::world& ecs) {
  ecs.module<LightModule>();
  pWorld_ = &ecs;

  InitializeComponent();
  InitializeObserver();
  InitializeSystem();
}

void LightModule::InitializeComponent() {
  PTR_ASSERT(pWorld_);
  /*
   * glm::vec3 color;
float intensity;
   */
  pWorld_->component<PointLight>()
      .member<float>("position", 3)
      .member<float>("color", 3)
      .member<float>("intensity");

  /*glm::vec3 direction;
    glm::vec3 color;
    float intensity;
   */
  pWorld_->component<DirectionalLight>()
      .member<float>("direction", 3)
      .member<float>("color", 3)
      .member<float>("intensity");

  /*
glm::vec3 position;
glm::vec3 direction;
glm::vec3 color;
float intensity;
float cutoff;
   */
  pWorld_->component<SpotLight>()
      .member<float>("position", 3)
      .member<float>("direction", 3)
      .member<float>("color", 3)
      .member<float>("intensity")
      .member<float>("cutoff");
}

void LightModule::InitializeObserver() { PTR_ASSERT(pWorld_); }

void LightModule::InitializeSystem() {
  PTR_ASSERT(pWorld_);
  /*  pWorld_->system<PointLight>("PointLightOnUpdate")
        .kind(Phase::LightBinding)
        .each(PointLightOnUpdate);

    pWorld_->system<DirectionalLight>("DirectionalLightOnUpdate")
        .kind(Phase::LightBinding)
        .each(DirectionalLightOnUpdate);

    pWorld_->system<SpotLight>("SpotLightOnUpdate")
        .kind(Phase::LightBinding)
        .each(SpotLightOnUpdate);*/
}

/*
 * if (group.groupedHandle == 0) {
glCreateBuffers(1, &group.groupedHandle);

glBindBuffer(GL_SHADER_STORAGE_BUFFER, group.groupedHandle);
glBufferData(GL_SHADER_STORAGE_BUFFER,
         static_cast<GLsizeiptr>(sizeof(glm::mat4) * group.size),
         nullptr, GL_DYNAMIC_DRAW);
Logger::get<TransformModule>()->info("Create buffer for transform group {}",
                                 group.groupedHandle);
}
if (group.groupedHandle == 0) {
 */
void LightModule::CreateLightBuffers() {
  PTR_ASSERT(pWorld_);

  // using storage buffer as to save the Light data, basically don't need GPU to
  // modified the data anymore
  Handle lightBufferGroup = 0;
  glCreateBuffers(1, &lightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizeiptr>(sizeof(PointLight) * 10), nullptr,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                   Uniform::BindingLocation::ePointLight, lightBufferGroup);

  Handle DirectionLightBufferGroup = 0;
  glCreateBuffers(1, &DirectionLightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, DirectionLightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizeiptr>(sizeof(DirectionalLight) * 10), nullptr,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                   Uniform::BindingLocation::eDirectionLight,
                   DirectionLightBufferGroup);

  Handle SpotLightBufferGroup = 0;
  glCreateBuffers(1, &SpotLightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, SpotLightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizeiptr>(sizeof(SpotLight) * 10), nullptr,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                   Uniform::BindingLocation::eSpotLight, SpotLightBufferGroup);
  //  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, group->groupedHandle);
}

MOD_END(LightModule)