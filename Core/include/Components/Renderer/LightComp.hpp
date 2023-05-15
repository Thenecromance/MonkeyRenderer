#pragma once

#include <glm/glm.hpp>

#include "Job.hpp"
#include "Marcos/Marcos.hpp"
//@section-start
COMP_BGN(LightComp)
USING(glm)
struct PointLight {
  vec3 position;  // instead of using PositionComp just for convinience to
                  // upload the data to GPU
  vec3 color;
  float intensity;
};
SIZE_OF(PointLight)

struct DirectionalLight {
  vec3 direction;
  vec3 color;
  float intensity;
};
SIZE_OF(DirectionalLight)

struct SpotLight {
  vec3 position;
  vec3 direction;
  vec3 color;
  float intensity;
  float cutoff;
};
SIZE_OF(SpotLight)

struct LightTransform {
  mat4 projection;
  mat4 matrices;
};
COMP_END(LightComp)