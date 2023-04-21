UI(Texture) {
  COM_START(Texture);
  SUB_DRAW(hHandle);  // Handle to the texture
  SUB_DRAW(Type);
  ImGui::Image((void*)(intptr_t)comp.hHandle, ImVec2(128, 128));
  COM_END();
};

UI(PBRTexture) {
  COM_START(PBRTexture);
  
  COM_END();
}