//
// Created by Thenecromance on 2023/3/1.
//
#include "Core.hpp"

#include <GLFW/glfw3.h>

#include "FileWatcherModule.hpp"
#include "Input/InputHandler.hpp"
#include "InputComponents.hpp"
#include "Logger.hpp"
#include "Marcos/Marcos.hpp"
#include "OpenGLApp.hpp"
ROOT_BGN()

bool Core::Initialize(float dt, int num_thread) {
  Logger::get("Core")->info("Core::Initialize");

  if (!OpenGLApp::GetInstance()->Initialize()) {
    Logger::get("Core")->error("OpenGL Initialize failed");
    return false;
  }

  { InputHandler::GetInstance()->Initialize(); }

  ecs.set_threads(1);
  ecs.set_target_fps(dt);

  return true;
}

bool Core::OnUpdate() {
  if (!OpenGLApp::GetInstance()->OnUpdateBegin()) return false;

  InputHandler::GetInstance()->OnUpdate();

  // run 1 ticks for ecs
  { ecs.progress(); }
  InputHandler::GetInstance()->ResetScroll();

  OpenGLApp::GetInstance()->OnUpdateEnd();
  //  FileWatcherModule::GetInstance()->OnUpdate();
  return true;
}

void Core::EnableRest() {
  ecs.set<flecs::Rest>({});
  ecs.import <flecs::monitor>();
  ecs.app().enable_rest().enable_monitor();
}

void Core::Release() { ecs.~world(); }

ROOT_END()