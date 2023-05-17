//
// Created by Thenecromance on 2023/4/14.
//
#include "ImGuiComponentDraw.hpp"

#include <imgui.h>

#include "Components.hpp"
using namespace Monkey::Component;

void Begin(const char* name) { ImGui::Begin(name); }
void End() { ImGui::End(); }

template <typename _Type>
void Element(_Type& comp);
#define STEP_FLOAT_MIN 0.1f
#define STEP_FLOAT_MAX 10.f
#define ELE_(type) \
  template <>      \
  void Element<type>(type & comp)

#include "BaseType.inl"

ELE_(glm::vec3) {
  ImGui::InputFloat("X", &comp.x, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
  ImGui::InputFloat("Y", &comp.y, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
  ImGui::InputFloat("Z", &comp.z, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
}
ELE_(glm::quat) {
  ImGui::InputFloat("X", &comp.x, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
  ImGui::InputFloat("Y", &comp.y, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
  ImGui::InputFloat("Z", &comp.z, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
  ImGui::InputFloat("W", &comp.w, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
}
ELE_(glm::vec2) {
  ImGui::InputFloat("X", &comp.x, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
  ImGui::InputFloat("Y", &comp.y, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
}
ELE_(Position) {
  ImGui::InputFloat("X", &comp.value.x, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
  ImGui::InputFloat("Y", &comp.value.y, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
  ImGui::InputFloat("Z", &comp.value.z, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
}
#undef ELE_

#define UI(type) \
  template <>    \
  void Draw(type& comp)
#define COM_START(name) if (ImGui::CollapsingHeader(#name)) {
#define COM_END() }

#define SUB_END()   \
  ImGui::TreePop(); \
  }
#define SUB_DRAW(type)          \
  if (ImGui::TreeNode(#type)) { \
    Element(comp.type);         \
    SUB_END();
#include "CameraComponent.inl"
#include "Light.inl"
#include "Model.inl"
#include "Position.inl"
#include "Texture.inl"

#undef UI
#undef COM_START
#undef COM_END
#undef SUB_END
#undef SUB_DRAW
