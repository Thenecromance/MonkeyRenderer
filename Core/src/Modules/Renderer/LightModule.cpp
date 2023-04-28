#include "LightModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include <OpenGLApp.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "LightComp.hpp"
#include "MeshComp.hpp"
#include "RenderComp.hpp"

using namespace Monkey::Component;
unsigned int lightHandle{};

// update light buffer
void OnPointLightUpdate(PointLight& light) {
  if (lightHandle == 0) {
    glCreateBuffers(1, &lightHandle);
    glNamedBufferStorage(lightHandle, sizeof(PointLight), nullptr,
                         GL_DYNAMIC_STORAGE_BIT);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightHandle, 0, sizeof(PointLight));
  }
  glNamedBufferSubData(lightHandle, 0, sizeof(PointLight), &light);
}

void LightUI(PointLight& light) {
  ImGui::Begin("Light");
  ImGui::InputFloat("Position X:", &light.position.x, 1.0f, 10.0f);
  ImGui::InputFloat("Position Y:", &light.position.y, 1.0f, 10.0f);
  ImGui::InputFloat("Position Z:", &light.position.z, 1.0f, 10.0f);

  ImGui::SliderFloat("Color R:", &light.color.r, 0.0f, 1.0f);
  ImGui::SliderFloat("Color G:", &light.color.g, 0.0f, 1.0f);
  ImGui::SliderFloat("Color B:", &light.color.b, 0.0f, 1.0f);
  ImGui::End();
}

LightModule::LightModule(world& ecs) {
  ecs.module<LightModule>();

  ecs.observer<PointLight>()
      .event(flecs::OnAdd)
      .each([](flecs::iter& it, size_t i, PointLight& light) {
        light.position = {1.0f, 1.0, 1.0};
        light.color = {1.0f, 1.0, 1.0};
        light.intensity = 1.0f;
      });

  // update Point light first
  ecs.system<PointLight>("PointLightUpdate")
      .kind(flecs::PostUpdate)
      .each(OnPointLightUpdate);
  //  // render shadow map
  //  ecs.system<ShadowMap>("ShadowMapUpdate")
  //      .kind(flecs::PreStore)  // here can be move to post update
  //      .each(ShadowMapUpdate);
  //
  //  ecs.entity("ShadowMapEntity").add<ShadowMap>().add<FrameBuffer>();

  ecs.system<PointLight>("PointLightUI").kind(flecs::PreUpdate).each(LightUI);
}