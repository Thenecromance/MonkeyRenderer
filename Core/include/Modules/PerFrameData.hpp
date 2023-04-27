//#pragma once
//
//#include "Components/CameraComponent.hpp"
//#include "Job.hpp"
//#include "Marcos/Marcos.hpp"
//#include "glm/glm.hpp"
//
//COMP_BGN(PerFrameDataComp)
//
//struct PerFrameDataComp {
//  glm::mat4 view;
//  glm::mat4 projection;
//  glm::mat4 model;
//};
//
//struct PerFrameDataHandle {
//  unsigned int handle{};
//};
//
//SIZE_OF(PerFrameDataComp);
//
//COMP_END(PerFrameDataComp)
//
//MOD_BGN(PerFrameData)
////
//// class PerFrameDataModule {
//// public:
////  explicit PerFrameDataModule(world &ecs);
////
////  static void PerFrameDataUpdater(
////      const Component::CameraComponent &cameraComponent);
////
//// private:
////  static void CreateBuffer();
////
//// public:
////  static unsigned int uiBufferHandle;
////  static Component::PerFrameDataComp
////      *pPerFrameDataComp;  // perframedata should be a component and don't
////      need
////                           // to do like this
////  // maybe other unique data should be a component too
////};
//
//class PerFrameDataModule {
// public:
//  explicit PerFrameDataModule(world& ecs);
//
// private:
//  static constexpr int bindinglocation = 1;
//};
//MOD_END(PerFrameData)