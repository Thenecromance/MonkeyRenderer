//
// Created by Thenecromance on 2023/3/1.
//

#ifndef MONKEYRENDER_CORE_HPP
#define MONKEYRENDER_CORE_HPP

#include <flecs.h>

#include "CommonDef.hpp"
#include "Graphics/OpenGLApp.hpp"
#include "Job.hpp"
#include "Marcos/Marcos.hpp"
#include "Patterns/SingleTon.hpp"

class OpenGLApp;

class PhaseBuilder;

ROOT_BGN()
class Core : public LazySingleTon<Core> {
 public:
  bool Initialize(float dt, int num_thread = 1);

  bool OnUpdate();

  template <class Module>
  Core &Import() {
    ecs.import <Module>();
    return *this;
  }

  /// @brief enable flecs's RestApi
  void EnableRest();

  world &GetWorld() { return ecs; }

  void Release();

 private:
  world ecs;
  Name *name{};
};

ROOT_END()

#endif  // MONKEYRENDER_CORE_HPP
