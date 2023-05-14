
#include <flecs.h>

#include "Marcos/Marcos.hpp"

MOD_BGN(DefferedRender)

class DefferedRender {
 public:
  explicit DefferedRender(flecs::world& ecs);
  void InitializeComponent();
  void InitializeObserver();
  void InitializePrefab();
  void ImportSystem();

 private:
  flecs::entity defaultGbuffer{};
  flecs::entity defaultLightPass{};
  flecs::entity defaultGeomPass{};
  flecs::entity preFab_{};
  flecs::world* pWorld_{};
};
MOD_END(DefferedRender)