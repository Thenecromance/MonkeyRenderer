
#include "TransformModule.hpp"

#include <glad/glad.h>
#include <imgui.h>

#include <Phases.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"
#include "GlobalValue.hpp"
#include "Logger.hpp"

MOD_BGN(TransformModule)

using namespace Component;

/// this method still has lots of ways to impove the performance, but not now :(
void TransformIter(flecs::iter& it, Transform* transform, Position* position_,
                   Scale* scale_, Rotation* rotation_) {
  auto self = it.entity(0);
  if (!self.has<TransformGroup>()) {
    Logger::get<TransformModule>()->error(
        "entity {} doesn't have transform "
        "group, please add it first",
        self.name());
    return;
  }

  auto group = self.get<TransformGroup>();

  for (auto idx : it) {
    // clang-format off
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, scale_[idx].value);

    model = glm::rotate(model, glm::radians(rotation_[idx].value.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation_[idx].value.y), glm::vec3(0.0f, 1.0f, 0.0f)); // y is not correct
    model = glm::rotate(model, glm::radians(rotation_[idx].value.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, position_[idx].value);  // model matrices transform


    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    transform[idx].value =model;
    // clang-format on
  }

  //  glBindBufferBase(GL_SHADER_STORAGE_BUFFER,Uniform::BindingLocation::eMatrices,
  //  group->groupedHandle); glNamedBufferSubData(group->groupedHandle, 0,
  //  sizeof(glm::mat4) * it.count(),self.get<Transform>());
}

TransformModule::TransformModule(world& ecs) {
  ecs.module<TransformModule>();
  pWorld_ = &ecs;
  LoadTransformGroup();

  LoadTransformComponent();

  ecs.system<Transform, Position, Scale, Rotation>("TransformUpdate")
      .kind(Phase::LogicUpdate)
      .iter(TransformIter);
}

void TransformModule::LoadTransformComponent() {
  prefab_ = pWorld_->prefab("TransformGroup")
                .add<TransformGroup>();  // using the same buffer handle to
                                         // update the transform group
  Logger::get("Debug")->info("{}", prefab_.path());
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
void TransformGroupAllocate(TransformGroup& group) {
  if (group.groupedHandle == 0) {
    // when use this way to upload data to the buffer , the limit is 65535
    // bytes(64kb) (on my hard ware) in other word, the max size of the object
    // is 1000
    glCreateBuffers(1, &group.groupedHandle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, group.groupedHandle);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 static_cast<GLsizeiptr>(sizeof(glm::mat4) * group.size),
                 nullptr, GL_DYNAMIC_DRAW);
    Logger::get<TransformModule>()->info("Create buffer for transform group {}",
                                         group.groupedHandle);
  }
  if (group.groupedHandle == 0) {
    Logger::get<TransformModule>()->error(
        "Failed to create buffer for transform group");
    assert(false && "Failed to create buffer for transform group");
  }
}

/// @brief so far this transform group is only has one buffer, but here will be
/// auto allocate a new buffer if the buffer is reached the max size
/// @param transformGroup TransformGroup component
void TransformGroupExtend(flecs::iter& it, size_t i, TransformGroup& group) {
  TransformGroupAllocate(group);
  group.bufferInUse++;
  if (group.bufferInUse >= group.size) {
    group.size *= 2;  // double the size
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, group.groupedHandle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * group.size,
                 nullptr, GL_DYNAMIC_DRAW);
  }

  it.world()
      .lookup("::Monkey::Module::TransformModule::TransformGroup")
      .set<TransformGroup>({group});
}

void TransformModule::LoadTransformGroup() {
  pWorld_->observer<TransformGroup>()
      .event(flecs::OnAdd)
      .each(TransformGroupExtend);
}

MOD_END(TransformModule)
