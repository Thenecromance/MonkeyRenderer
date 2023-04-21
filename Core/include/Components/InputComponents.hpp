#pragma once

#include "Marcos/Marcos.hpp"

#define KEY_SIZE 12  // with no padding
enum eMouseButton {
  eLeft,
  eRight,
  eMiddle,
  eExtra1,
  eExtra2,
  eExtra3,
  eExtra4,
  eExtra5,
  eButtonEnd
};

COMP_BGN(Input)

struct KeyBoard {
  //	bool bPressed[349]; //this will mapping all key state, but it's waste
  // too much memory
  short int bPressed[KEY_SIZE];  // now this will save key code which is pressed
};

///@brief cursorPos Component
struct CursorPos {
  double x, y;
};

///@brief Mouse Button Component
struct MouseButton {
  //    int button, action, mods;
  bool bPressed[eMouseButton::eButtonEnd];
};

///@brief Scroll Component
struct ScrollButton {
  double xoffset, yoffset;
};

/// @brief this component has include all control stuff, so it will no more need
/// lots of component
struct InputController {
  KeyBoard keyBoard;
  CursorPos cursorPos;
  MouseButton mouseButton;
  ScrollButton scrollButton;
};

COMP_END(Input)