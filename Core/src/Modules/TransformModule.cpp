
#include "TransformModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

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
    glNamedBufferSubData(transform.Matrices, 0, sizeof(glm::mat4),
    value_ptr(m));
}

/*template <typename Comp_>
void AddTransForm(flecs::entity self, Comp_&) {

  if (self.has<Transform>()) return;


  Logger::get<TransformModule>()->trace("{} Initalize Transform",
                                        self.name().c_str());

  Handle handle = 0;
  glCreateBuffers(1, &handle);
  glNamedBufferStorage(handle, sizeof(glm::mat4), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  //  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, handle);
  if (handle != 0) {
    self.set<Transform>({.Matrices = handle});
  }
}*/

/*void AddTransFormFromPosition(flecs::iter& it, size_t i, Position&) {
  auto self = it.entity(i);
  if (self.has<Transform>()) return;

  Logger::get<TransformModule>()->trace("{} Add Transform from Position",
                                        self.name().c_str());
  Handle handle = 0;
  glCreateBuffers(1, &handle);
  glNamedBufferStorage(handle, sizeof(glm::mat4), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  if (handle != 0) {
    self.set<Transform>({.Matrices = handle});
  }
}
void AddTransFormFromScale(flecs::iter& it, size_t i, Scale&) {
  auto self = it.entity(i);
  if (self.has<Transform>()) return;
  self.add<Transform>();
  Logger::get<TransformModule>()->trace("{} Add Transform from Scale",
                                        self.name().c_str());
  Handle handle = 0;
  glCreateBuffers(1, &handle);
  glNamedBufferStorage(handle, sizeof(glm::mat4), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  if (handle != 0) {
    self.set<Transform>({.Matrices = handle});
  }
}
void AddTransFormFromRotation(flecs::iter& it, size_t i, Rotation&) {
  auto self = it.entity(i);
  if (self.has<Transform>()) return;
  Logger::get<TransformModule>()->trace("{} Add Transform from Rotation",
                                        self.name().c_str());
  Handle handle = 0;
  glCreateBuffers(1, &handle);
  glNamedBufferStorage(handle, sizeof(glm::mat4), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  if (handle != 0) {
    self.set<Transform>({.Matrices = handle});
  }
}*/

void InitializeTransform(flecs::entity e, Transform& transform) {
  glCreateBuffers(1, &transform.Matrices);
  glNamedBufferStorage(transform.Matrices, sizeof(glm::mat4), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
}

void LoadComponent(world& ecs) {
  ecs.component<Transform>().member<Handle>("handle");

  ecs.component<Position>().member<float>("x").member<float>("y").member<float>(
      "z");
  ecs.component<Rotation>()
      .member<float>("x")
      .member<float>("y")
      .member<float>("z")
      .member<float>("angle");
  ecs.component<Scale>().member<float>("x").member<float>("y").member<float>(
      "z");
}

TransformModule::TransformModule(world& ecs) {
  ecs.module<TransformModule>();
  LoadComponent(ecs);
  ecs.system<Monkey::Component::Transform>("TransformUpdate")
      .each(TransformSystem);

  ecs.observer<Transform>().event(flecs::OnAdd).each(InitializeTransform);
}

MOD_END(TransformModule)