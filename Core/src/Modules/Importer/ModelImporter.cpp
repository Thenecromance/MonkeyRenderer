
#include "ModelImporter.hpp"

#include <flecs.h>

#include "Logger.hpp"
#include "Model/BaseModel.hpp"
#include "Position.hpp"
#include "Renderer/BaseRenederModule.hpp"
#include "Renderer/DefferedRender.hpp"
#include "Renderer/ForwardRenderModule.hpp"

using namespace Monkey::Component;

bool ModelHasRenderer(flecs::entity& self) {
  if (!self.has<ForwardRenderer>() || !self.has<DefferedRenderer>() ||
      !self.has<BaseRenderer>()) {
    return false;
  }
  return true;
}
void ModelOnSet(flecs::iter& it, size_t i, Model& model) {
  auto self = it.entity(i);
  if (!ModelHasRenderer(self)) {
    Logger::get("Model")->trace("Model {} has no renderer, add default",
                                self.name());
    self.add<ForwardRenderer>();  // TODO: need to check before, if model has
                                  // position , rotation etc, make it as
                                  // default renderer
  }
  self.add<Position>().add<Rotation>().add<Scale>();
}

void ModelOnRemove(flecs::iter& it, size_t i, Model& model) {
  auto self = it.entity(i);
  if (ModelHasRenderer(self)) {
    // TODO:disable renderer first , to avoid the wrong render result
  }
}

void ModelUnSet(flecs::iter& it, size_t i, Model& model) {}

ModelImporter::ModelImporter(world& ecs) {
  ecs.module<ModelImporter>();

  //  ecs.observer<Model>().each(ModelOnAdd);
  ecs.observer<Model>().event(flecs::OnSet).each(ModelOnSet);
  ecs.observer<Model>().event(flecs::UnSet).each(ModelUnSet);
  ecs.observer<Model>().event(flecs::OnRemove).each(ModelOnRemove);
}
