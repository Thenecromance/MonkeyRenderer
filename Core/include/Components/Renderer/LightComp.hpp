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

// force align to 16 bytes
struct PointLight {
  __declspec(align(16)) float intensity{};  // 4 bytes
  __declspec(align(16)) vec3 position;      // 12 bytes
  __declspec(align(16)) vec3 color;         // 12 bytes
  __declspec(align(16)) vec4 Angle;         // 16 bytes
};
SIZE_OF(PointLight)

struct DirectionalLight {
  __declspec(align(16)) vec3 direction;
  __declspec(align(16)) vec3 color;
  __declspec(align(16)) float intensity;
};
SIZE_OF(DirectionalLight)

struct SpotLight {
  __declspec(align(16)) vec3 position;
  __declspec(align(16)) vec3 direction;
  __declspec(align(16)) vec3 color;
  __declspec(align(16)) float intensity;
  __declspec(align(16)) float cutoff;
};
SIZE_OF(SpotLight)

struct LightTransform {
  mat4 view; // glm::mat4 is the same as the GPU mat4 so don't need to be changed
  mat4 projection;
};
SIZE_OF(LightTransform)
COMP_END(LightComp)