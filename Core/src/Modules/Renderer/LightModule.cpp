#include "LightModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>


#include "GlobalValue.hpp"
#include "LightComp.hpp"
#include "MeshComp.hpp"
#include "Phases.hpp"

MOD_BGN(LightModule)
using namespace Monkey::Component;

// need to find a way to upload light data to gpu

LightModule::LightModule(flecs::world& ecs) {
  ecs.module<LightModule>();
  pWorld_ = &ecs;

  InitializeComponent();
  CreateLightBuffers();
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
  pWorld_->system<PointLight>("PointLightOnUpdate")
      .kind(Phase::LightBinding)
      .iter([&](flecs::iter& it) {
        // ignore the ClangTidy warning:Narrowing conversion from 'size_t' (aka
        // 'unsigned long long') to signed type 'GLsizeiptr' (aka 'long long')
        // is implementation-defined I don't think anyone can have a GPU that
        // has more than 2^63 bytes of memory at least so far nobody can reach
        // that
        glNamedBufferSubData(PointLightBufferGroup, 0,
                             sizePointLight * it.count(),
                             it.entity(0).get<PointLight>());
      });

  pWorld_->system<DirectionalLight>("DirectionalLightOnUpdate")
      .kind(Phase::LightBinding)
      .iter([&](flecs::iter& it) {
        glNamedBufferSubData(PointLightBufferGroup, 0,
                             sizeDirectionalLight * it.count(),
                             it.entity(0).get<DirectionalLight>());
      });

  pWorld_->system<SpotLight>("SpotLightOnUpdate")
      .kind(Phase::LightBinding)
      .iter([&](flecs::iter& it) {
        glNamedBufferSubData(PointLightBufferGroup, 0,
                             sizeSpotLight * it.count(),
                             it.entity(0).get<SpotLight>());
      });
}

void LightModule::CreateLightBuffers() {
  PTR_ASSERT(pWorld_);

  // using storage buffer as to save the Light data, basically don't need GPU to
  // modified the data anymore
  glCreateBuffers(1, &PointLightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, PointLightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizeiptr>(sizeof(PointLight) * poolSize), nullptr,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                   Uniform::BindingLocation::ePointLight,
                   PointLightBufferGroup);

  glCreateBuffers(1, &DirectionLightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, DirectionLightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizeiptr>(sizeof(DirectionalLight) * poolSize),
               nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                   Uniform::BindingLocation::eDirectionLight,
                   DirectionLightBufferGroup);

  glCreateBuffers(1, &SpotLightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, SpotLightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<GLsizeiptr>(sizeof(SpotLight) * poolSize), nullptr,
               GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                   Uniform::BindingLocation::eSpotLight, SpotLightBufferGroup);
}

MOD_END(LightModule)