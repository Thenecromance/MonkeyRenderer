
ELE_(bool) { ImGui::Checkbox("Active Camera", &comp); }
ELE_(float) { ImGui::InputFloat("X", &comp, STEP_FLOAT_MIN, STEP_FLOAT_MAX); }
ELE_(int) {
  ImGui::InputInt("value", &comp, (int)STEP_FLOAT_MIN, (int)STEP_FLOAT_MAX);
}
ELE_(unsigned int) {
  ImGui::InputInt("value", (int*)&comp, (int)STEP_FLOAT_MIN,
                  (int)STEP_FLOAT_MAX);
}
