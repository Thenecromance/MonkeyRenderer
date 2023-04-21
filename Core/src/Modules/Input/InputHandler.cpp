//
// Created by Thenecromance on 2023/1/31.
//

#include "InputHandler.hpp"

#include <glfw/glfw3.h>

#include <cassert>

#include "Components/InputComponents.hpp"
#include "Logger.hpp"
#include "OpenGLApp.hpp"
#include "imgui.h"

// in msvc x64 and x86 keyoffset is different
#if _WIN64
#define KEY_OFFSET 144
#else
#define KEY_OFFSET 120
#endif
#define MOUSE_OFFSET KEY_OFFSET - 8

using namespace Monkey::Component;

// double  InputHandler::scroll_x = 0.0;
// double  InputHandler::scroll_y = 0.0;
//
//
// void InputHandler::ScrollCallBack(GLFWwindow *ptr, double xoffset, double
// yoffset) { 	scroll_x = xoffset; 	scroll_y = yoffset;
// }
//
//// here need an operator to remapping OpenGLKey to ImGui ....
// void InputHandler::OnUpdate() {
//	assert(pWindow && "pWindow could not be nullptr ");
//
//	// if the window is not focused , just don't need to handle the input
// etc 	if (!glfwGetWindowAttrib((GLFWwindow *) pWindow, GLFW_FOCUSED))
// return;
//	//get cursor pos from opengl
//	glfwGetCursorPos((GLFWwindow *) pWindow, &cursor_x, &cursor_y);
//
//
//	{
//		scroll_x = 0;
//		scroll_y = 0;
//	}
//	//	// use  memory copy to get the key data
//	//	memcpy(keys, (char *) pWindow + 120, 349);
// }
//
//
//
// void InputHandler::Initialize(void *window_ptr) {
//	assert(window_ptr);
//
//	this->pWindow = window_ptr;
//
//	Logger::get("InputHandler")->info("Bind input handler to window {}",
// pWindow);
//
//	// use a pointer to direct get the key raw data
//	{
//		keys.ptr = ((unsigned char *) pWindow +
//		            KEY_OFFSET); // key data is pWindow +120bytes offset
// or maybe using the WindowPadding
//
//	}
//	// mouse button mirror
//	{
//		mouse_button.ptr = ((unsigned char *) pWindow + MOUSE_OFFSET);
//	}
//	// using scroll callback to get scroll data
//	{
//		glfwSetScrollCallback((GLFWwindow *) pWindow, ScrollCallBack);
//	}
//
//	//temp add a key callback for quit the window
//	{
//		glfwSetKeyCallback((GLFWwindow *) pWindow, [](GLFWwindow
//*window, int key, int scancode, int action, int mods) {
//			/*	ImGuiIO &io = ImGui::GetIO();
//				io.KeysDown[key] = (action != GLFW_RELEASE);
//				io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL]
//||
// io.KeysDown[GLFW_KEY_RIGHT_CONTROL]; io.KeyShift =
// io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
//				io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] ||
// io.KeysDown[GLFW_KEY_RIGHT_ALT]; 				io.KeySuper =
// io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];*/
//
//			if (key == GLFW_KEY_ESCAPE) {
//				glfwSetWindowShouldClose(window, true);
//			}
//		});
//	}
// }
//
////void InputHandler::InitializeKey() {
////	// from key 0 to key F12 all in the same place , just use this to build
/// mapper /	for (int i = ImGuiKey_0; i <= ImGuiKey_F12; ++i) { /
/// mImGuiToKey[i] = GLFW_KEY_0 + i - ImGuiKey_0; /	} /	// the same as
/// above /	for (int i = ImGuiKey_Keypad0; i <= ImGuiKey_Keypad9; ++i) { /
/// mImGuiToKey[i] = GLFW_KEY_KP_0 + i - ImGuiKey_Keypad0; /	} /	//
/// because most of the function keys ImGui and GLFW have totally different key
/// name, so it could be only use this fool ways to generate it /
/// mImGuiToKey[ImGuiKey_None] = GLFW_KEY_UNKNOWN; /
/// mImGuiToKey[ImGuiKey_Tab] = GLFW_KEY_TAB; /	mImGuiToKey[ImGuiKey_LeftArrow]
///= GLFW_KEY_LEFT; /	mImGuiToKey[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT; /
/// mImGuiToKey[ImGuiKey_UpArrow] = GLFW_KEY_UP; /
/// mImGuiToKey[ImGuiKey_DownArrow] = GLFW_KEY_DOWN; /
/// mImGuiToKey[ImGuiKey_Home] = GLFW_KEY_HOME;
////	mImGuiToKey[ImGuiKey_End] = GLFW_KEY_END;
////	mImGuiToKey[ImGuiKey_Insert] = GLFW_KEY_INSERT;
////	mImGuiToKey[ImGuiKey_Delete] = GLFW_KEY_DELETE;
////	mImGuiToKey[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
////	mImGuiToKey[ImGuiKey_Space] = GLFW_KEY_SPACE;
////	mImGuiToKey[ImGuiKey_Enter] = GLFW_KEY_ENTER;
////	mImGuiToKey[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
////	mImGuiToKey[ImGuiKey_LeftCtrl] = GLFW_KEY_LEFT_CONTROL;
////	mImGuiToKey[ImGuiKey_LeftShift] = GLFW_KEY_LEFT_SHIFT;
////	mImGuiToKey[ImGuiKey_LeftAlt] = GLFW_KEY_LEFT_ALT;
////	mImGuiToKey[ImGuiKey_LeftSuper] = GLFW_KEY_LEFT_SUPER;
////	mImGuiToKey[ImGuiKey_RightCtrl] = GLFW_KEY_RIGHT_CONTROL;
////	mImGuiToKey[ImGuiKey_RightShift] = GLFW_KEY_RIGHT_SHIFT;
////	mImGuiToKey[ImGuiKey_RightAlt] = GLFW_KEY_RIGHT_ALT;
////	mImGuiToKey[ImGuiKey_RightSuper] = GLFW_KEY_RIGHT_SUPER;
////	mImGuiToKey[ImGuiKey_Menu] = GLFW_KEY_MENU;
////	mImGuiToKey[ImGuiKey_Apostrophe] = GLFW_KEY_APOSTROPHE;         // '
////	mImGuiToKey[ImGuiKey_Comma] = GLFW_KEY_COMMA;                 // ,
////	mImGuiToKey[ImGuiKey_Minus] = GLFW_KEY_MINUS;                 // -
////	mImGuiToKey[ImGuiKey_Period] = GLFW_KEY_PERIOD;                 // .
////	mImGuiToKey[ImGuiKey_Slash] = GLFW_KEY_SLASH;                 // /
////	mImGuiToKey[ImGuiKey_Semicolon] = GLFW_KEY_SEMICOLON;         // ;
////	mImGuiToKey[ImGuiKey_Equal] = GLFW_KEY_EQUAL;                 // =
////	mImGuiToKey[ImGuiKey_LeftBracket] = GLFW_KEY_LEFT_BRACKET;     // [
////	mImGuiToKey[ImGuiKey_Backslash] = GLFW_KEY_BACKSLASH;         // \ (this
/// text inhibit multiline comment caused by backslash) /
/// mImGuiToKey[ImGuiKey_RightBracket] = GLFW_KEY_RIGHT_BRACKET; // ] /
/// mImGuiToKey[ImGuiKey_GraveAccent] = GLFW_KEY_GRAVE_ACCENT;     // ` /
/// mImGuiToKey[ImGuiKey_CapsLock] = GLFW_KEY_CAPS_LOCK; /
/// mImGuiToKey[ImGuiKey_ScrollLock] = GLFW_KEY_SCROLL_LOCK; /
/// mImGuiToKey[ImGuiKey_NumLock] = GLFW_KEY_NUM_LOCK; /
/// mImGuiToKey[ImGuiKey_PrintScreen] = GLFW_KEY_PRINT_SCREEN; /
/// mImGuiToKey[ImGuiKey_Pause] = GLFW_KEY_PAUSE; /
/// mImGuiToKey[ImGuiKey_KeypadDecimal] = GLFW_KEY_KP_DECIMAL; /
/// mImGuiToKey[ImGuiKey_KeypadDivide] = GLFW_KEY_KP_DIVIDE; /
/// mImGuiToKey[ImGuiKey_KeypadMultiply] = GLFW_KEY_KP_MULTIPLY; /
/// mImGuiToKey[ImGuiKey_KeypadSubtract] = GLFW_KEY_KP_SUBTRACT; /
/// mImGuiToKey[ImGuiKey_KeypadAdd] = GLFW_KEY_KP_ADD; /
/// mImGuiToKey[ImGuiKey_KeypadEnter] = GLFW_KEY_KP_ENTER; /
/// mImGuiToKey[ImGuiKey_KeypadEqual] = GLFW_KEY_KP_EQUAL;
////}
//
//[[maybe_unused]] bool InputHandler::GetKey(int key_code) {
//	return keys[key_code] == GLFW_PRESS;
//}
//
// void InputHandler::HandleKeyBoard(KeyBoard &keyBoard) {
//	//	memcpy(keyBoard.bPressed, *keys.ptr, sizeof(keyBoard.bPressed));
//
//	memset(keyBoard.bPressed, 0, sizeof(keyBoard.bPressed));
//	int idx = 0;
//	for (short i = 32; i < 349; ++i) {
//		if (keys[i] == GLFW_PRESS) {
//			keyBoard.bPressed[idx] = i;
//			idx++;
//			// the keyboard comp is full
//			if (idx == KEY_SIZE) {
//				return;
//			}
//		}
//	}
//}
//
// void InputHandler::HandleMouseButton(MouseButton &mouseButton) {
//	// copy the data from the MouseButtonMapper
//	for (int i = 0; i < eButtonEnd; ++i) {
//		mouseButton.bPressed[i] = mouse_button[i];
//	}
//}
//
// void InputHandler::HandleCursorPos(CursorPos &cursorPos) const {
//	cursorPos.x = cursor_x;
//	cursorPos.y = cursor_y;
//}
//
// void InputHandler::HandleScrollButton(ScrollButton &scrollButton) {
//	scrollButton.xoffset = scroll_x;
//	scrollButton.yoffset = scroll_y;
//}
//

void InputHandler::ScrollCallBack(GLFWwindow *ptr, double xoffset,
                                  double yoffset) {
  /*	scroll_x = xoffset;
          scroll_y = yoffset;*/

  InputHandler::GetInstance()->SetScroll(xoffset, yoffset);
}

void InputHandler::SetScroll(double x, double y) {
  scroll_x = x;
  scroll_y = y;
}

void InputHandler::Initialize() {
  auto pWindow = OpenGLApp::GetInstance()->GetWindow();
  Logger::get<InputHandler>()->info("Bind input handler to window {}", pWindow);

  // use a pointer to direct get the key raw data
  {
    keys.ptr = ((unsigned char *)pWindow +
                128);  // key data is pWindow +120bytes offset or maybe
                       // using the WindowPadding
  }
  // mouse button mirror
  { mouse_button.ptr = ((unsigned char *)pWindow + 120); }
  // using scroll callback to get scroll data
  { glfwSetScrollCallback((GLFWwindow *)pWindow, ScrollCallBack); }
  // temp add a key callback for quit the window
  {
    glfwSetKeyCallback(
        (GLFWwindow *)pWindow,
        [](GLFWwindow *window, int key, int scancode, int action, int mods) {
          if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
          }
        });
  }
}

void InputHandler::OnUpdate() {
  const auto pWindow = OpenGLApp::GetInstance()->GetWindow();
  assert(pWindow && "pWindow could not be nullptr ");
  // if the window is not focused , just don't need to handle the input
  if (!glfwGetWindowAttrib((GLFWwindow *)pWindow, GLFW_FOCUSED)) return;
  // get cursor pos from opengl
  glfwGetCursorPos((GLFWwindow *)pWindow, &cursor_x, &cursor_y);
}

void InputHandler::HandleKeyBoard(KeyBoard &keyBoard) {
  //	memcpy(keyBoard.bPressed, *keys.ptr, sizeof(keyBoard.bPressed));
  memset(keyBoard.bPressed, 0, sizeof(keyBoard.bPressed));
  int idx = 0;
  for (short i = 32; i < 349; ++i) {
    if (keys[i] == GLFW_PRESS) {
      keyBoard.bPressed[idx] = i;
      idx++;
      // the keyboard comp is full
      if (idx == KEY_SIZE) {
        return;
      }
    }
  }
}

void InputHandler::HandleMouseButton(MouseButton &mouseButton) {
  // copy the data from the MouseButtonMapper
  std::memcpy(mouseButton.bPressed, mouse_button.ptr, 5);
}

void InputHandler::HandleCursorPos(CursorPos &cursorPos) const {
  cursorPos.x = cursor_x;
  cursorPos.y = cursor_y;
}

void InputHandler::HandleScrollButton(ScrollButton &scrollButton) const {
  scrollButton.xoffset = scroll_x;
  scrollButton.yoffset = scroll_y;
}

void InputHandler::ResetScroll() {
  scroll_x = 0;
  scroll_y = 0;
}
