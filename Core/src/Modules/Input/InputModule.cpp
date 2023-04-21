#include "InputModule.hpp"

#include <flecs.h>

#include "InputComponents.hpp"
#include "InputHandler.hpp"
#include "Logger.hpp"

using namespace Monkey::Component;

MOD_BGN(Input)

void OnHandleKeyBoard(KeyBoard &keyBoard) {
  InputHandler::GetInstance()->HandleKeyBoard(keyBoard);
}

void OnHandleMouseButton(MouseButton &mouseButton) {
  InputHandler::GetInstance()->HandleMouseButton(mouseButton);
}

void OnHandleCursorPos(CursorPos &cursorPos) {
  InputHandler::GetInstance()->HandleCursorPos(cursorPos);
}

void OnHandleScrollButton(ScrollButton &scrollButton) {
  InputHandler::GetInstance()->HandleScrollButton(scrollButton);
}

void OnHandleInputController(InputController &inputController) {
  OnHandleKeyBoard(inputController.keyBoard);
  OnHandleMouseButton(inputController.mouseButton);
  OnHandleCursorPos(inputController.cursorPos);
  OnHandleScrollButton(inputController.scrollButton);
}

InputModule::InputModule(world &ecs) {
  ecs.module<InputModule>();

  Logger::get("Module")->trace("{} Loaded", __FUNCTION__);

  ecs.component<KeyBoard>().member<short int>("keyPressed", KEY_SIZE);

  ecs.component<CursorPos>().member<double>("x").member<double>("y");
  ecs.component<MouseButton>().member<bool>("bPressed",
                                            eMouseButton::eButtonEnd);

  ecs.component<ScrollButton>().member<double>("xoffset").member<double>(
      "yoffset");

  ecs.component<InputController>()
      .member<KeyBoard>("keyBoard")
      .member<CursorPos>("cursorPos")
      .member<MouseButton>("MouseButton")
      .member<ScrollButton>("scrollButton");

  ecs.system<KeyBoard>("OnHandleKeyBoard")
      .kind(flecs::OnLoad)
      .each(OnHandleKeyBoard);
  ecs.system<CursorPos>("OnHandleCursorPos")
      .kind(flecs::OnLoad)
      .each(OnHandleCursorPos);
  ecs.system<MouseButton>("OnHandleMouseButton")
      .kind(flecs::OnLoad)
      .each(OnHandleMouseButton);
  ecs.system<ScrollButton>("OnHandleScrollButton")
      .kind(flecs::OnLoad)
      .each(OnHandleScrollButton);
  ecs.system<InputController>("OnHandleInputController")
      .kind(flecs::OnLoad)
      .each(OnHandleInputController);
}

MOD_END(Input)
