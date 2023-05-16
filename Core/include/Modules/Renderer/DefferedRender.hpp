
#include <flecs.h>

#include "Marcos/Marcos.hpp"

MOD_BGN(DefferedRender)

class DefferedRender {
 public:
  explicit DefferedRender(flecs::world& ecs);
  void InitializeComponent();
  void InitializeObserver();
  void InitializeDefault();
  void ImportSystem();

 private:
  flecs::entity prefab_{};
  flecs::entity defaultLightPass{};
  flecs::entity defaultGeomPass{};
  flecs::world* pWorld_{};
};
MOD_END(DefferedRender)