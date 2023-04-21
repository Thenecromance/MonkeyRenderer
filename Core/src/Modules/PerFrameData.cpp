
#include "PerFrameData.hpp"

#include <glad/glad.h>
#include <imgui.h>

#include "Logger.hpp"
#include "flecs.h"

using namespace Monkey::Component;
MOD_BGN(PerFrameData)

glm::mat4 getViewMatrix(glm::vec3 position, glm::quat Orientation);

unsigned int PerFrameDataModule::uiBufferHandle = 0;
Component::PerFrameDataComp *PerFrameDataModule::pPerFrameDataComp = nullptr;

void test(flecs::iter &it) {}
PerFrameDataModule::PerFrameDataModule(world &ecs) {
  ecs.module<PerFrameDataModule>();
  Logger::get("Module")->trace("{} Loaded", __FUNCTION__);

  CreateBuffer();
  pPerFrameDataComp = new Component::PerFrameDataComp();

  ecs.system<const Component::CameraComponent>("PerFrameDataUpdater")
      .kind(flecs::OnStore)
      .each(PerFrameDataUpdater);
}

void PerFrameDataModule::PerFrameDataUpdater(
    const CameraComponent &cameraComponent) {
  const glm::mat4 v =
      getViewMatrix(cameraComponent.v3Position.value, cameraComponent.Orientation);
  const glm::mat4 p = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
  pPerFrameDataComp->view = v;
  pPerFrameDataComp->projection = p;
  pPerFrameDataComp->cameraPos = glm::vec4(cameraComponent.v3Position.value, 1.0f);
  glNamedBufferSubData(uiBufferHandle, 0, sizePerFrameDataComp,
                       pPerFrameDataComp);
}

void PerFrameDataModule::CreateBuffer() {
  glCreateBuffers(1, &uiBufferHandle);
  glNamedBufferStorage(uiBufferHandle, sizePerFrameDataComp, nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uiBufferHandle, 0,
                    sizePerFrameDataComp);
  Logger::get<PerFrameDataModule>()->info("PerFrameData {}", uiBufferHandle);
}

MOD_END(PerFrameData)
