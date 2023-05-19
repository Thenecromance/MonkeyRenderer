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
  glm::vec3 value{1.0f, 0.0f, 0.0f};
  float angle{0.0f};
};
SIZE_OF(Rotation)

struct Scale {
  glm::vec3 value{1.0f, 1.0f, 1.0f};
};
SIZE_OF(Scale);

struct Transform {
  glm::mat4 value{};
};

SIZE_OF(Transform);

struct Projection {
  glm::mat4 value{};
};

struct TransformGroup {
  Handle groupedHandle{0};
  int size{1};
  int bufferInUse{0};
};
COMP_END(Position)