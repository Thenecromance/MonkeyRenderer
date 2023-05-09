
#include "TransformModule.hpp"

#include <glad/glad.h>

#include <Phases.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"
#include "Logger.hpp"

MOD_BGN(TransformModule)

using namespace Component;

// TODO: finish the transform system, this just a basic demo ver
void TransformSystem(entity self, Monkey::Component::Transform& transform) {
  auto scale_ = glm::vec3(1.0f);
  auto rotation_ = glm::vec3(1.0f);
  auto position_ = glm::vec3(1.0f);
  float angle_ = 0.0f;
  //  if (self.has<Scale>()) scale_ = self.get<Scale>()->value;
  if (self.has<Position>()) position_ = self.get<Position>()->value;
  if (self.has<Rotation>()) {
    rotation_ = self.get<Rotation>()->value;
    angle_ = self.get<Rotation>()->angle;
  }

  const glm::mat4 scale = glm::scale(glm::mat4(1.0f), scale_);
  const glm::mat4 rotation =
      glm::rotate(glm::mat4(1.0f), glm::radians(angle_), rotation_);
  const glm::mat4 pos = glm::translate(glm::mat4(1.0f), position_);

  const glm::mat4 m =
      glm::rotate(scale * rotation * pos, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
  transform.value = m;
  glNamedBufferSubData(transform.Matrices, 0, sizeof(glm::mat4), value_ptr(m));
}

void InitializeTransform(flecs::entity e, Transform& transform) {
  glCreateBuffers(1, &transform.Matrices);
  glNamedBufferStorage(transform.Matrices, sizeof(glm::mat4), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
}

void LoadComponent(world& ecs) {
  // clang-format off
  ecs.component<Transform>()
      .member<Handle>("handle");

  ecs.component<Position>()
      .member<float>("x")
      .member<float>("y")
      .member<float>("z");

  ecs.component<Rotation>()
      .member<float>("x")
      .member<float>("y")
      .member<float>("z")
      .member<float>("angle");

  ecs.component<Scale>()
      .member<float>("x")
      .member<float>("y")
      .member<float>("z");
  // clang-format on
}

TransformModule::TransformModule(world& ecs) {
  ecs.module<TransformModule>();
  LoadComponent(ecs);

  // clang-format off
  ecs.system<Monkey::Component::Transform>("TransformUpdate")
      .kind(Phase::LogicUpdate)
      .each(TransformSystem);

  ecs.observer<Transform>()
      .event(flecs::OnAdd)
      .each(InitializeTransform);
  // clang-format on
}

MOD_END(TransformModule)
