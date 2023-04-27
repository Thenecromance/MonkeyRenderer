
#include "PerFrameData.hpp"

#include <glad/glad.h>
#include <imgui.h>

#include "Logger.hpp"
#include "OpenGLApp.hpp"
#include "flecs.h"

using namespace Monkey::Component;
MOD_BGN(PerFrameData)
#pragma region deprecated
//
//// this part module should be deprecated
//
// glm::mat4 getViewMatrix(glm::vec3 position, glm::quat Orientation);
//
// unsigned int PerFrameDataModule::uiBufferHandle = 0;
// Component::PerFrameDataComp *PerFrameDataModule::pPerFrameDataComp = nullptr;
//
// PerFrameDataModule::PerFrameDataModule(world &ecs) {
//  ecs.module<PerFrameDataModule>();
//  Logger::get("Module")->trace("{} Loaded", __FUNCTION__);
//
//  CreateBuffer();
//  pPerFrameDataComp = new Component::PerFrameDataComp();
//
//  ecs.system<const Component::CameraComponent>("PerFrameDataUpdater")
//      .kind(flecs::OnStore)
//      .each(PerFrameDataUpdater);
//}
//
// void PerFrameDataModule::PerFrameDataUpdater(
//    const CameraComponent &cameraComponent) {
//  const glm::mat4 v =
//      getViewMatrix(cameraComponent.v3Position.value,
//      cameraComponent.Orientation);
//  const glm::mat4 p = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
//  pPerFrameDataComp->view = v;
//  pPerFrameDataComp->projection = p;
//  pPerFrameDataComp->cameraPos =
//  glm::vec4(cameraComponent.v3Position.value, 1.0f);
//  glNamedBufferSubData(uiBufferHandle, 0, sizePerFrameDataComp,
//                       pPerFrameDataComp);
//}
//
// void PerFrameDataModule::CreateBuffer() {
//  glCreateBuffers(1, &uiBufferHandle);
//  glNamedBufferStorage(uiBufferHandle, sizePerFrameDataComp, nullptr,
//                       GL_DYNAMIC_STORAGE_BIT);
//  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uiBufferHandle, 0,
//                    sizePerFrameDataComp);
//  Logger::get<PerFrameDataModule>()->info("PerFrameData {}", uiBufferHandle);
//}
#pragma endregion

// buggged
void PerframeDataUpdateSystem(CameraComponent& camComp) {
  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);

  const glm::mat4 projection =
      glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
}

// perframe data buffer
void CreateBuffer(PerFrameDataHandle& perframeDataBuffer) {
  glCreateBuffers(1, &perframeDataBuffer.handle);
  glNamedBufferStorage(perframeDataBuffer.handle, sizePerFrameDataComp, nullptr,
                       GL_DYNAMIC_STORAGE_BIT);

  //(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr
  // size);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, perframeDataBuffer.handle, 0,
                    sizePerFrameDataComp);
  // glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);
 
}

PerFrameDataModule::PerFrameDataModule(world& ecs) {
  ecs.module<PerFrameDataModule>();

  ecs.component<PerFrameDataComp>().on_add(
      [](flecs::entity e, PerFrameDataComp& comp) {
        comp.model = glm::mat4(1.0f);
        comp.view = glm::mat4(1.0f);
        comp.projection = glm::mat4(1.0f);
        e.add<PerFrameDataHandle>();
      });

  ecs.component<PerFrameDataHandle>().on_add(CreateBuffer);
}

MOD_END(PerFrameData)
