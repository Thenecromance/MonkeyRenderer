#pragma once
#include <map>

#include "Marcos/Marcos.hpp"
#include "Patterns/SingleTon.hpp"

struct GLFWwindow;

class InputHandler;

ROOT_BGN()
class Core;
ROOT_END()

#define MAPPER(name, min_value, max_value)      \
  class name {                                  \
    friend InputHandler;                        \
                                                \
   public:                                      \
    int operator[](int key) {                   \
      if (key < min_value || key > max_value) { \
        return INT_MIN;                         \
      }                                         \
      return ptr[key];                          \
    }                                           \
                                                \
   private:                                     \
    unsigned char *ptr{};                       \
  }

// change the pointer to the key array
MAPPER(KeyBoardMapper, 32, 349);
// change the pointer to the mouse button array
MAPPER(MouseButtonMapper, 0, 8);

// forward declare
namespace Monkey::Component {
struct KeyBoard;
struct MouseButton;
struct CursorPos;
struct ScrollButton;
};  // namespace Monkey::Component

using namespace Monkey::Component;

// 48 bytes....
class InputHandler : public LazySingleTon<InputHandler> {
  friend Monkey::Core;

 protected:
  void Initialize();

  static void ScrollCallBack(GLFWwindow *ptr, double xoffset, double yoffset);

  void SetScroll(double x, double y);

 public:
  /// @brief using this way to get key state, cursor state when each tick start
  void OnUpdate();

  void ResetScroll();

  void HandleKeyBoard(KeyBoard &keyBoard);

  void HandleMouseButton(MouseButton &mouseButton);

  void HandleCursorPos(CursorPos &cursorPos) const;

  void HandleScrollButton(ScrollButton &scrollButton) const;

 private:
  // ImGui key ReMapping
  //	void InitializeKey();

 private:
  double cursor_x{};
  double cursor_y{};

  double scroll_x{};
  double scroll_y{};

  MouseButtonMapper mouse_button;
  KeyBoardMapper keys;  // max value is 349 //4bytes

  std::map<int, int> mImGuiToKey;
};
