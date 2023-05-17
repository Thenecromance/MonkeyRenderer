
UI(PointLight) {
  COM_START(PointLight);
  SUB_DRAW(position);  // instead of using PositionComp just for convinience to
                       // upload the data to GPU
  SUB_DRAW(color);
  SUB_DRAW(intensity);
  COM_END();
};

UI(DirectionalLight) {
  COM_START(DirectionalLight);
  SUB_DRAW(direction);
  SUB_DRAW(color);
  SUB_DRAW(intensity);
  COM_END();
};

UI(SpotLight) {
  COM_START(SpotLight);
  SUB_DRAW(position);
  SUB_DRAW(direction);
  SUB_DRAW(color);
  SUB_DRAW(intensity);
  SUB_DRAW(cutoff);
  COM_END();
};
