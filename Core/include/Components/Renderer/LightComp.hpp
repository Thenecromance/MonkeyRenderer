#pragma once

#include <glm/glm.hpp>

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

COMP_BGN(LightComp)

struct PointLight {
  glm::vec3 position;
  glm::vec3 color;
  float intensity;
};
SIZE_OF(PointLight)

struct DirectionalLight {
  glm::vec3 direction;
  glm::vec3 color;
  float intensity;
};
SIZE_OF(DirectionalLight)

struct SpotLight {
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 color;
  float intensity;
  float cutoff;
};
SIZE_OF(SpotLight)

struct LightTransform {
  glm::mat4 projection{};
  glm::mat4 matrices{};
};
COMP_END(LightComp)