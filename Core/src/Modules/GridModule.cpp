#include "GridModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include "CommonDef.hpp"
#include "GridComponent.hpp"
#include "Job.hpp"
#include "Logger.hpp"
#include "ShaderLoader.hpp"

/*
as what I want to do this module, an grid should be a component, when I need it
, just import this module,then everything should be working, and when I don't
need it, just remove it, and other modules should not be affected
*/

MOD_BGN(Grid)

void OnUpdateGridRender(const Component::GridComponent& Comp) {
  glUseProgram(Comp.handle);
  glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
}

void InitializeGrid(flecs::iter& it, size_t i, Component::GridComponent& grid) {
  grid.handle = ProgramLoader::GetInstance()->LoadAndLink(
      {"Shaders/Grid/grid.vs", "Shaders/Grid/grid.fs"}, "GridProgram");

  Logger::get<GridModule>()->info("GridProgram {} Loaded", grid.handle);
}

void UnLoadGrid(flecs::iter& it, size_t i, Component::GridComponent& grid) {
  ProgramLoader::GetInstance()->Erase(
      ProgramLoader::GetInstance()->GetUID("GridProgram"));
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Logger::get<GridModule>()->info("GridProgram {} Unloaded", grid.handle);
}

GridModule::GridModule(world& ecs) {
  ecs.module<GridModule>();

  ecs.component<Component::GridComponent>().member<Handle>("handle");

  ecs.system<const Component::GridComponent>("GridUpdater")
      .kind(flecs::OnStore)
      .each(OnUpdateGridRender);

  ecs.observer<Component::GridComponent>()
      .event(flecs::OnAdd)
      .event(flecs::OnSet)
      .each(InitializeGrid);

  ecs.observer<Component::GridComponent>()
      .event(flecs::OnRemove)
      .event(flecs::UnSet)
      .each(UnLoadGrid);

  ecs.entity("Grid").add<Component::GridComponent>();

  Logger::get<GridModule>()->info("GridModule Loaded");
}

MOD_END(Grid)