UI(Position) {
  COM_START(Position);
  SUB_DRAW(value);
  COM_END();
};

/*template <>
void Draw(Position& comp) {
  if (ImGui::CollapsingHeader("Position")) {
    if (ImGui::TreeNode("value")) {
      *//*Element(comp.value);*//*
        ImGui::InputFloat("X", &comp.value.x, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
        ImGui::InputFloat("Y", &comp.value.y, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
        ImGui::InputFloat("Z", &comp.value.z, STEP_FLOAT_MIN, STEP_FLOAT_MAX);
      ImGui::TreePop();
    }
  }
}*/

UI(Rotation) {
  COM_START(Rotation);
  SUB_DRAW(value);
  COM_END();
};

UI(Scale) {
  COM_START(Scale);
  SUB_DRAW(value);
  COM_END();
};
