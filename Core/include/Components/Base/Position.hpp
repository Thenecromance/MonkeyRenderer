#pragma once

#include <glm/glm.hpp>

#include "CommonDef.hpp"
#include "Marcos/Marcos.hpp"

COMP_BGN(Position)
struct Position {
  glm::vec3 value{1.0f, 1.0f, 1.0f};
};
// using Position = glm::vec3;
SIZE_OF(Position)
struct Rotation {
  glm::vec3 value{0.0f, 0.0f, 1.0f};
  float angle{180.0f};
};
SIZE_OF(Rotation)

struct Scale {
  glm::vec3 value{1.0f, 1.0f, 1.0f};
};
SIZE_OF(Scale);

struct Transform {
  Handle Matrices{};
  glm::mat4 value;
};

SIZE_OF(Transform);

struct Projection {
  glm::mat4 value{};
};
COMP_END(Position)