#include "LightModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include "Renderer/LightComp.hpp"

using namespace Monkey::Component;
unsigned int lightHandle{};
void OnPointLightUpdate(PointLight& light) {
  
  glDisable(GL_BLEND);
  if (lightHandle == 0) {
    glCreateBuffers(1, &lightHandle);
    glNamedBufferStorage(lightHandle, sizeof(PointLight), nullptr,
                         GL_DYNAMIC_STORAGE_BIT);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightHandle, 0, sizeof(PointLight));
  }

  glNamedBufferSubData(lightHandle, 0, sizeof(PointLight), &light);
  glEnable(GL_BLEND);
}
void LightUI(PointLight& light) {
  ImGui::Begin("Light");
  ImGui::InputFloat("Position X:", &light.position.x, 1.0, 10.0);
  ImGui::InputFloat("Position Y:", &light.position.y, 1.0, 10.0);
  ImGui::InputFloat("Position Z:", &light.position.z, 1.0, 10.0);

  ImGui::InputFloat("Color R:", &light.color.r, 0.1, 1.0f);
  ImGui::InputFloat("Color G:", &light.color.g, 0.1, 1.0f);
  ImGui::InputFloat("Color B:", &light.color.b, 0.1, 1.0f);
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

  ecs.system<PointLight>("PointLightUpdate")
      .kind(flecs::PreStore)
      .each(OnPointLightUpdate);

  ecs.system<PointLight>("PointLightUI").kind(flecs::PreUpdate).each(LightUI);
}