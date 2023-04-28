//
// Created by Thenecromance on 2023/4/19.
//

#include "AntiAliasingConfigModule.hpp"

#include <Phases.hpp>
#include <glad/glad.h>
#include <imgui.h>

#include "glfw/glfw3.h"
MOD_BGN(AntiAliasingConfigModule)
void UIUpdate(AntiAliasingConfig& config) {
  ImGui::Begin("AntiAliasingConfig");
  if (ImGui::Checkbox("Enable MSAA", &config.enableMSAA)) {
    if (config.enableMSAA) {
      glfwWindowHint(GLFW_SAMPLES, config.msaaLevel);
      glEnable(GL_MULTISAMPLE);
    } else {
      glDisable(GL_MULTISAMPLE);
      glfwWindowHint(GLFW_SAMPLES, 1);
    }
  }
  if (config.enableMSAA) {
    if (ImGui::SliderInt("MSAA Level", &config.msaaLevel, 1, 16)) {
      glfwWindowHint(GLFW_SAMPLES, config.msaaLevel);
    }
  }
  ImGui::End();
}
AntiAliasingConfigModule::AntiAliasingConfigModule(world& ecs) {
  ecs.module<AntiAliasingConfigModule>();

  ecs.system<AntiAliasingConfig>("AntiAliasingConfigSystem")
      .kind(Phase::ImGuiRender)
      .each(UIUpdate);

  ecs.entity("AAConfig")
      .set<AntiAliasingConfig>(
          {.enableMSAA = false, .enableFXAA = false, .msaaLevel = 4});
}
MOD_END(AntiAliasingConfigModule)