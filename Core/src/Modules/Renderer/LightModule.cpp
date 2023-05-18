#include "LightModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>
#include <glm/ext.hpp>

#include "GlobalValue.hpp"
#include "LightComp.hpp"
#include "Logger.hpp"
#include "MeshComp.hpp"
#include "Phases.hpp"
#include "RenderComp.hpp"

MOD_BGN(LightModule)
using namespace Monkey::Component;





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
      .each([&](flecs::iter& it, size_t i, PointLight& light) {
        if (it.event() == flecs::OnAdd) {
          point_light_count_++;
        }
      });
}

void LightModule::InitializeSystem() {
  PTR_ASSERT(pWorld_);

  // so far Light System only update each Lights' data from local
  {
    pWorld_->system<PointLight>("PointLightOnUpdate")
        .kind(Phase::LightBinding)
        .iter([&](flecs::iter& it, PointLight* light) {
          for(auto row:it ){
            light[row].position.z += .1f;
            if(light[row].position.z > 100.f){
              light[row].position.z = -20.f;
            }
          }
          glNamedBufferSubData(PointLightBufferGroup, 0,static_cast<GLsizeiptr>(sizePointLight * it.count()), light);
        });

    pWorld_->system<DirectionalLight>("DirectionalLightOnUpdate")
        .kind(Phase::LightBinding)
        .iter([&](flecs::iter& it, DirectionalLight* light) {
          glNamedBufferSubData( PointLightBufferGroup, 0,static_cast<GLsizeiptr>(sizeDirectionalLight * it.count()),light);
         
        });

    pWorld_->system<SpotLight>("SpotLightOnUpdate")
        .kind(Phase::LightBinding)
        .iter([&](flecs::iter& it, SpotLight* light) {
          glNamedBufferSubData( PointLightBufferGroup, 0,static_cast<GLsizeiptr>(sizeSpotLight * it.count()), light);
         
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


glBindBufferBase(GL_SHADER_STORAGE_BUFFER,Uniform::BindingLocation::ePointLight,PointLightBufferGroup);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER,Uniform::BindingLocation::eDirectionLight,DirectionLightBufferGroup);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER,Uniform::BindingLocation::eSpotLight, SpotLightBufferGroup);
  // clang-format on
  Logger::get<LightModule>()->debug(
      "Create Light Buffers, PointLight:{0} , DirectionLight:{1} , "
      "SpotLight:{2}",
      PointLightBufferGroup, DirectionLightBufferGroup, SpotLightBufferGroup);
}

MOD_END(LightModule)