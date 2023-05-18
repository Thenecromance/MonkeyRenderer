#include "LightModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include "GlobalValue.hpp"
#include "LightComp.hpp"
#include "Logger.hpp"
#include "MeshComp.hpp"
#include "Phases.hpp"
#include "RenderComp.hpp"

MOD_BGN(LightModule)
using namespace Monkey::Component;

#define STEP_FLOAT_MIN 1.0f
#define STEP_FLOAT_MAX 10.f

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

void LightModule::InitializeObserver() {
  PTR_ASSERT(pWorld_);
  pWorld_->observer<PointLight>("PointLight")
      .event(flecs::OnAdd)
      .event(flecs::OnSet)
      .each([](flecs::iter& it, size_t i, PointLight& light) {
        if (it.event() == flecs::OnAdd) it.entity(i).add<LightTransform>();
      });
}

void LightModule::InitializeSystem() {
  PTR_ASSERT(pWorld_);

  // so far Light System only update each Lights' data from local
  {
    pWorld_->system<PointLight>("PointLightOnUpdate")
        .kind(Phase::LightBinding)
        .iter([&](flecs::iter& it, PointLight* light) {
          glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                           Uniform::BindingLocation::ePointLight,
                           PointLightBufferGroup);
          glNamedBufferSubData(
              PointLightBufferGroup, 0,
              static_cast<GLsizeiptr>(sizePointLight * it.count()), &light[0]);
          point_light_count_ = it.count();
        });

    pWorld_->system<DirectionalLight>("DirectionalLightOnUpdate")
        .kind(Phase::LightBinding)
        .iter([&](flecs::iter& it, DirectionalLight* light) {
          glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                           Uniform::BindingLocation::eDirectionLight,
                           DirectionLightBufferGroup);
          glNamedBufferSubData(
              PointLightBufferGroup, 0,
              static_cast<GLsizeiptr>(sizeDirectionalLight * it.count()),
              light);
          direction_light_count_ = it.count();
        });

    pWorld_->system<SpotLight>("SpotLightOnUpdate")
        .kind(Phase::LightBinding)
        .iter([&](flecs::iter& it, SpotLight* light) {
          glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                           Uniform::BindingLocation::eSpotLight,
                           SpotLightBufferGroup);
          glNamedBufferSubData(
              PointLightBufferGroup, 0,
              static_cast<GLsizeiptr>(sizeSpotLight * it.count()), light);
          spot_light_count_ = it.count();
        });
  }
  // just sync to perframe data , this info will be upload to gpu later
  pWorld_->system<PerFrameDataComp>("LightInfoUpdate")
      .kind(Phase::LightBinding)
      .each([&](flecs::entity self, PerFrameDataComp& data) {
        data.point_light_count = point_light_count_;
        data.direction_light_count = direction_light_count_;
        data.spot_light_count = spot_light_count_;
      });
  // Light Matrices
  {
//    pWorld_->system<PointLight, LightTransform>("LightTransform")
//        .kind(Phase::LogicUpdate)
//        .each([](flecs::iter& it, size_t i , PointLight& light , LightTransform& transform) {
//
//        });
  }
}

void LightModule::CreateLightBuffers() {
  PTR_ASSERT(pWorld_);

  // using storage buffer as to save the Light data, basically don't need GPU to
  // modified the data anymore
  // clang-format off
  glCreateBuffers(1, &PointLightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, PointLightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER,static_cast<GLsizeiptr>(sizeof(PointLight) * poolSize), nullptr,GL_DYNAMIC_DRAW);

  glCreateBuffers(1, &DirectionLightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, DirectionLightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER,static_cast<GLsizeiptr>(sizeof(DirectionalLight) * poolSize),nullptr, GL_DYNAMIC_DRAW);

  glCreateBuffers(1, &SpotLightBufferGroup);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, SpotLightBufferGroup);
  glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(sizeof(SpotLight) * poolSize), nullptr,GL_DYNAMIC_DRAW);

  // clang-format on
  Logger::get<LightModule>()->debug(
      "Create Light Buffers, PointLight:{0} , DirectionLight:{1} , "
      "SpotLight:{2}",
      PointLightBufferGroup, DirectionLightBufferGroup, SpotLightBufferGroup);
}

MOD_END(LightModule)