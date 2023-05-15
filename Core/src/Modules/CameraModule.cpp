//
// Created by Thenecromance on 2023/3/12.
//

#include "CameraModule.hpp"

// #include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include <Phases.hpp>

#include "CameraComponent.hpp"
#include "GlobalValue.hpp"
#include "ImGuiComponentDraw.hpp"
#include "InputComponents.hpp"
#include "Logger.hpp"
#include "OpenGLApp.hpp"
#include "RenderComp.hpp"
#include "glfw/glfw3.h"

/*
struct PerFrameData {
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec4 cameraPos;
};
*/

MOD_BGN(Camera)
using namespace Component;
CamSetting camSetting;

glm::mat4 getViewMatrix(glm::vec3 position, glm::quat Orientation) {
  const glm::mat4 t = glm::translate(glm::mat4(1.0f), -position);
  const glm::mat4 r = glm::mat4_cast(Orientation);
  return r * t;
}

auto setUpVector(CameraComponent &cameraComponent) {
  const glm::vec3 pos = cameraComponent.v3Position.value;
  const glm::mat4 view = getViewMatrix(pos, cameraComponent.Orientation);
  const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);

  cameraComponent.Orientation = glm::lookAt(pos, pos + dir, cameraComponent.up);
};

void OnCameraUpdate(flecs::entity e, CameraComponent &cameraComponent,
                    const InputController &inputController) {
  auto deltaSeconds = e.delta_time();
  int width, height;
  glfwGetWindowSize((GLFWwindow *)OpenGLApp::GetInstance()->GetWindow(), &width,
                    &height);

  glm::vec3 accel(0.0f);
  const glm::vec2 mousePos_ =
      glm::vec2(static_cast<float>(inputController.cursorPos.x / width),
                static_cast<float>(inputController.cursorPos.y / height));
  // camera rotation by mouse
  if (inputController.mouseButton.bPressed[eMouseButton::eRight]) {
    // calculate the camera rotation
    const glm::vec2 delta =
        mousePos_ - cameraComponent.mousePos;  // new - old position

    /*			if (glm::zero<glm::vec2>() == delta)
                                                    return;*/

    const glm::quat deltaQuat =
        glm::quat(glm::vec3(-camSetting.mouseSpeed_ * delta.y,
                            camSetting.mouseSpeed_ * delta.x, 0.0f));
    cameraComponent.Orientation = deltaQuat * cameraComponent.Orientation;
    cameraComponent.Orientation = glm::normalize(cameraComponent.Orientation);
    setUpVector(cameraComponent);
  }

  // update the mouse position for next frame using
  cameraComponent.mousePos = mousePos_;

  const glm::mat4 v = glm::mat4_cast(cameraComponent.Orientation);
  const glm::vec3 forward = -glm::vec3(v[0][2], v[1][2], v[2][2]);
  const glm::vec3 right = glm::vec3(v[0][0], v[1][0], v[2][0]);
  const glm::vec3 up = glm::cross(right, forward);

  bool fastSpeed_ = false;
  for (auto key : inputController.keyBoard.bPressed) {
    if (key == GLFW_KEY_W) accel += forward;
    if (key == GLFW_KEY_S) accel -= forward;
    if (key == GLFW_KEY_A) accel -= right;
    if (key == GLFW_KEY_D) accel += right;
    if (key == GLFW_KEY_Q) accel += up;
    if (key == GLFW_KEY_E) accel -= up;
    if (key == GLFW_KEY_LEFT_SHIFT) {
      fastSpeed_ = true;
      accel *= camSetting.fastCoef_;
    }
  }

  if (accel == glm::vec3(0)) {
    // decelerate naturally according to the damping value
    cameraComponent.moveSpeed -= cameraComponent.moveSpeed *
                                 std::min((1.0f / camSetting.damping_) *
                                              static_cast<float>(deltaSeconds),
                                          1.0f);
  } else {
    // acceleration
    cameraComponent.moveSpeed +=
        accel * camSetting.acceleration_ * static_cast<float>(deltaSeconds);
    const float maxSpeed = fastSpeed_
                               ? camSetting.maxSpeed_ * camSetting.fastCoef_
                               : camSetting.maxSpeed_;
    if (glm::length(cameraComponent.moveSpeed) > maxSpeed)
      cameraComponent.moveSpeed =
          glm::normalize(cameraComponent.moveSpeed) * maxSpeed;
  }

  cameraComponent.v3Position.value +=
      cameraComponent.moveSpeed * static_cast<float>(deltaSeconds);
}

void CameraUI(flecs::entity e, CameraComponent &cameraComponent) {
  Begin(e.name());
  Draw(cameraComponent);
  Draw(camSetting);
  End();
}

CameraModule::CameraModule(world &ecs) {
  ecs.import <CameraModule>();
  Logger::get("Module")->trace("{} Loaded", __FUNCTION__);

  ecs.component<CameraComponent>()
      .member<float>("Position_", 3)
      .member<float>("Orientation_", 4)
      .member<float>("up_", 3)
      .member<float>("mousePos_", 2)
      .member<float>("moveSpeed_", 3);

  ecs.component<CameraComponent>()
      .on_add([](flecs::entity e,
                 CameraComponent &cameraComponent) {  // default value
        cameraComponent.v3Position.value = glm::vec3(0.0f, 10.0f, 10.0f);
        cameraComponent.Orientation = glm::quat(glm::vec3(0));
        cameraComponent.up = glm::vec3(0.0f, 0.0f, 1.0f);
        cameraComponent.mousePos = glm::vec2(0);
        cameraComponent.moveSpeed = glm::vec3(0.0f);
        Logger::get("MonkeyTrace")
            ->info("CameraComponent added to entity {}", e.name());
        e.add<PerFrameDataComp>();
      })
      .on_set([](flecs::entity e, CameraComponent &cameraComponent) {
        e.add<PerFrameDataComp>();
      });

  ecs.system<CameraComponent, const InputController>("OnCameraUpdate")
      .kind(Phase::CameraUpdate)
      .each(OnCameraUpdate);

  ecs.system<CameraComponent>("CameraDebug")
      .kind(Phase::ImGuiRender)
      .each(CameraUI);

  PerFrameDataModule(ecs);
}

void PerFrameDataInit(PerFrameDataComp &comp) {
  glCreateBuffers(1, &comp.buffer);
  glNamedBufferStorage(comp.buffer, sizePerFrameDataComp - sizeof(Handle),
                       nullptr, GL_DYNAMIC_STORAGE_BIT);

  glBindBufferRange(GL_UNIFORM_BUFFER, Uniform::BindingLocation::ePerFrameData,
                    comp.buffer, 0, sizePerFrameDataComp - sizeof(Handle));
}
void PerFrameDataUpdate(PerFrameDataComp &comp,
                        const CameraComponent &camComp) {
  if (camComp.isActiveCamera == false) return;

  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);

  comp.view = getViewMatrix(camComp.v3Position.value, camComp.Orientation);
  comp.projection =
      glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
  comp.cameraPos = glm::vec4(camComp.v3Position.value, 1.0f);

  glNamedBufferSubData(comp.buffer, 0, sizePerFrameDataComp - sizeof(Handle),
                       &comp);
}

void CameraModule::PerFrameDataModule(world &ecs) {
  ecs.component<PerFrameDataComp>();

  ecs.observer<PerFrameDataComp>().event(flecs::OnAdd).each(PerFrameDataInit);
  ecs.system<PerFrameDataComp, const CameraComponent>(
         "PerFrameDataUpdateSystem")
      .kind(Phase::CameraUpdate)
      .each(PerFrameDataUpdate);
}
MOD_END(Camera)