//
// Created by Thenecromance on 2023/3/12.
//

#ifndef MONKEYRENDER_CAMERAMODULE_HPP
#define MONKEYRENDER_CAMERAMODULE_HPP

#include "Job.hpp"
#include "Marcos/Marcos.hpp"

MOD_BGN(Camera)

class CameraModule {
 public:
  explicit CameraModule(world &ecs);
  
 private:
  void PerFrameDataModule(world& ecs) ;
};

MOD_END(Camera)

#endif  // MONKEYRENDER_CAMERAMODULE_HPP
