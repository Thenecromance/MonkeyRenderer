
#include "TransformModule.hpp"

#include <glad/glad.h>

#include <Phases.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"
#include "Logger.hpp"

MOD_BGN(TransformModule)

using namespace Component;
/*
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
}*/

/// @brief base logic update for transform component
/// @param self the owner for tranform component
/// @param transform tranform component
void TransformUpdate(entity self, Transform& transform) {
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
}

TransformModule::TransformModule(world& ecs) {
  ecs.module<TransformModule>();
  pWorld_ = &ecs;

  LoadTransformGroup();

  LoadTransformComponent();

  ecs.system<Transform>("TransformUpdate")
      .kind(Phase::LogicUpdate)
      .each(TransformUpdate);
}

// void TransformOnAdd(entity self, Transform& transform) {
//   if (!self.has<TransformGroup>()) {
//     self.add<TransformGroup>();
//   }
// }

void TransformModule::LoadTransformComponent() {
  prefab_ = pWorld_->prefab("TransformGroup")
                .add<TransformGroup>();  // using the same buffer handle to
                                         // update the transform group

  // so far use the same buffer handle to update the transform group
  pWorld_->observer<Transform>("TranformOnAdd")
      .event(flecs::OnAdd)
      .each(
          // clang-format off
            [&](entity self, Transform& transform)
                  {
                    self.is_a(prefab_);
                  }

          // clang-format on
      );
}

/// @brief if the group is full then allocate a new buffer
/// @param self
/// @param group the transform group
void TransformGroupAllocate(entity self, TransformGroup& group) {
  if (group.groupedHandle == 0) {
    glCreateBuffers(1, &group.groupedHandle);
    // create an empty buffer for the group
    glNamedBufferStorage(group.groupedHandle, sizeof(glm::mat4), nullptr,
                         GL_DYNAMIC_STORAGE_BIT);
  }
  if (group.groupedHandle == 0) {
    Logger::get<TransformModule>()->error(
        "Failed to create buffer for transform group");
    assert(false && "Failed to create buffer for transform group");
  }
}

/// @brief just extend the size of the transform group
/// @param self
/// @param transformGroup
void TransformGroupExtend(entity self, TransformGroup& group) {
  TransformGroupAllocate(self, group);
  group.bufferInUse++;
  if (group.bufferInUse >= group.size) {
    group.size *= 2;  // double the size
    glNamedBufferStorage(group.groupedHandle, sizeof(glm::mat4) * group.size,
                         nullptr, GL_DYNAMIC_STORAGE_BIT);
  }
}

void TransformModule::LoadTransformGroup() {
  pWorld_->component<TransformGroup>()
      .member<Handle>("GroupHandle")
      .member<int>("Size")
      .member<int>("bufferInUse");
  // when transform group is on add the group buffer should be in a right way to
  // use
  auto TransformGroupExtend = [&](entity self, TransformGroup& group) {
    TransformGroupAllocate(self, group);
    group.bufferInUse++;
    if (group.bufferInUse >= group.size) {
      group.size *= 2;  // double the size
      glNamedBufferStorage(group.groupedHandle, sizeof(glm::mat4) * group.size,
                           nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
  };

  pWorld_->component<TransformGroup>()
      .on_add(TransformGroupExtend)
      .on_set([](entity self, TransformGroup&) {
        Logger::get<TransformModule>()->error("TransformGroup can not be set");
        assert(false && "TransformGroup can not be set");
      });
}

MOD_END(TransformModule)
