#pragma once

#include <glm/glm.hpp>

#include "Job.hpp"
#include "Marcos/Marcos.hpp"
//@section-start
COMP_BGN(LightComp)
USING(glm)

// in opengl vec3 has 16 bytes, so need to add padding to make it 16 bytes
// aligned in the same question, float has 4 bytes, so need to add padding to
// make it 16 bytes aligned or just all of them use vec4
struct PointLight {
  vec3 position;     // 12 bytes
  float padding0{};  // 4 bytes

  vec3 color;        // 12 bytes
  float padding1{};  // 4 bytes

  float intensity;  // 4 bytes
  vec3 padding2{};
};
SIZE_OF(PointLight)

struct DirectionalLight {
  vec3 direction;
  float padding0{};  // 4 bytes
  vec3 color;
  float padding1{};  // 4 bytes
  float intensity;
  vec3 padding2{};
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