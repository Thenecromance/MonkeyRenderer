UI(CameraComponent) {
  COM_START(CameraComponent);
  SUB_DRAW(v3Position);
  SUB_DRAW(Orientation);
  SUB_DRAW(up);
  SUB_DRAW(mousePos);
  SUB_DRAW(moveSpeed);
  SUB_DRAW(isActiveCamera);
  COM_END();
};

UI(CamSetting) {
  COM_START(CamSetting);
  SUB_DRAW(mouseSpeed_);
  SUB_DRAW(acceleration_);
  SUB_DRAW(damping_);
  SUB_DRAW(maxSpeed_);
  SUB_DRAW(fastCoef_);
  COM_END();
};

UI(CamMovement) {
  COM_START(CamMovement);
  SUB_DRAW(forward_);
  SUB_DRAW(backward_);
  SUB_DRAW(left_);
  SUB_DRAW(right_);
  SUB_DRAW(up_);
  SUB_DRAW(down_);
  SUB_DRAW(fastSpeed_);
  COM_END();
};