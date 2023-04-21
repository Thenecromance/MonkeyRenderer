//
// Created by Thenecromance on 2023/3/12.
//

#ifndef MONKEYRENDER_CAMERACOMPONENT_HPP
#define MONKEYRENDER_CAMERACOMPONENT_HPP

#include <CommonDef.hpp>
#include <Marcos/Marcos.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Components/Base/Position.hpp"  // temp to put here

COMP_BGN(Camera)
struct CameraComponent {
  Position v3Position;
  glm::quat Orientation = glm::quat(glm::vec3(0));
  glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec2 mousePos = glm::vec2(0);
  glm::vec3 moveSpeed = glm::vec3(0.0f);
  bool isActiveCamera = false;
};
SIZE_OF(CameraComponent)

struct CamMovement {
  bool forward_ = false;
  bool backward_ = false;
  bool left_ = false;
  bool right_ = false;
  bool up_ = false;
  bool down_ = false;
  bool fastSpeed_ = false;
};

SIZE_OF(CamMovement)

struct CamSetting {
  float mouseSpeed_ = 4.0f;
  float acceleration_ = 150.0f;
  float damping_ = 0.2f;
  float maxSpeed_ = 10.0f;
  float fastCoef_ = 10.0f;
};

SIZE_OF(CamSetting)

COMP_END(Camera)

#endif  // MONKEYRENDER_CAMERACOMPONENT_HPP
