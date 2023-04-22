#include "GridModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include <iostream>

#include "CommonDef.hpp"
#include "GridComponent.hpp"
#include "Job.hpp"
#include "Logger.hpp"
#include "ShaderComp.hpp"
#include "ShaderLoader.hpp"
/*
as what I want to do this module, an grid should be a component, when I need it
, just import this module,then everything should be working, and when I don't
need it, just remove it, and other modules should not be affected
*/

MOD_BGN(Grid)
using namespace Component;
void OnUpdateGridRender(const GridComponent& Comp) {
  NONE_ZERO_CHECK(Comp.handle);
  glEnable(GL_BLEND);

  glBindVertexArray(Comp.vao);
  glUseProgram(Comp.handle);
  glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
  glDisable(GL_BLEND);
}

void UnLoadGrid(flecs::iter& it, size_t i, GridComponent& grid) {
  glDeleteVertexArrays(1, &grid.vao);
  glDeleteProgram(grid.handle);
}

GridModule::GridModule(world& ecs) {
  ecs.module<GridModule>();
  ecs.component<GridComponent>().member<Handle>("handle");
  ecs.system<const GridComponent>("GridUpdater")
      .kind(flecs::OnStore)
      .each(OnUpdateGridRender);

  ecs.observer<Component::GridComponent>()
      .event(flecs::OnRemove)
      .each(UnLoadGrid);

  auto grid_handle = ecs.entity("gird").set<ShaderFile>(
      {"Shaders/Grid/grid.vs", "Shaders/Grid/grid.fs"});

  glCreateVertexArrays(
      1, &ecs.entity("Grid")
              .set<GridComponent>({grid_handle.get<Program>()->handle})
              .get_ref<GridComponent>()
              ->vao);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  Logger::get<GridModule>()->info("GridModule Loaded");
}

MOD_END(Grid)