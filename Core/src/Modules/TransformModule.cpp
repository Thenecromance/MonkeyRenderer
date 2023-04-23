
#include "TransformModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"


//TODO: finish the transform system, this just a basic demo ver
void TransformSystem(entity self, Monkey::Component::Transform& transform) {
  auto pScale = self.get<Monkey::Component::Scale>();
  glm::vec3 scale_ = pScale ? pScale->value : glm::vec3(1.0f);

  auto pRotation = self.get<Monkey::Component::Rotation>();
  glm::vec3 rotation_ = pRotation ? pRotation->value : glm::vec3(1.0f);
  float angle_ = pRotation ? pRotation->angle : 0.0f;

  auto pPosition = self.get<Monkey::Component::Position>();
  glm::vec3 position_ = pPosition ? pPosition->value : glm::vec3(1.0f);

  const glm::mat4 scale = glm::scale(glm::mat4(1.0f), scale_);
  const glm::mat4 rotation =
      glm::rotate(glm::mat4(1.0f), glm::radians(angle_), rotation_);
  const glm::mat4 pos = glm::translate(glm::mat4(1.0f), position_);

  const glm::mat4 m =
      glm::rotate(scale * rotation * pos, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));

  glNamedBufferSubData(transform.Matrices, 0, sizeof(glm::mat4), value_ptr(m));
}

TransformModule::TransformModule(world& ecs) {
  ecs.module<TransformModule>();

  ecs.system<Monkey::Component::Transform>("TransformUpdate")
      .each(TransformSystem);
}