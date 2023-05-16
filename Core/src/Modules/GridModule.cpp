#include "GridModule.hpp"

// #include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include <Phases.hpp>
#include <iostream>

#include "CommonDef.hpp"
#include "GridComponent.hpp"
#include "Job.hpp"
#include "Logger.hpp"
#include "ShaderComp.hpp"
/*
as what I want to do this module, an grid should be a component, when I need it
, just import this module,then everything should be working, and when I don't
need it, just remove it, and other modules should not be affected
*/

MOD_BGN(Grid)
using namespace Component;
void OnUpdateGridRender(const GridComponent& Comp, Program& program) {
  NONE_ZERO_CHECK(program.handle);
  glEnable(GL_BLEND);

  glBindVertexArray(Comp.vao);
  glUseProgram(program.handle);
  glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
  glDisable(GL_BLEND);
}

void UnLoadGrid(flecs::iter& it, size_t i, GridComponent& grid,
                Program& program) {
  glDeleteVertexArrays(1, &grid.vao);
  glDeleteProgram(program.handle);
}

GridModule::GridModule(world& ecs) {
  ecs.module<GridModule>();
  ecs.component<GridComponent>().member<Handle>("handle");
  ecs.system<const GridComponent, Program>("GridUpdater")
      .kind(Phase::RenderStage)
      .each(OnUpdateGridRender);

  ecs.observer<Component::GridComponent, Program>()
      .event(flecs::OnRemove)
      .each(UnLoadGrid);

  // clang-format off
  auto grid = ecs.entity("Grid")
                  .add<GridComponent>()
                  .set<ShaderFile>({"Shaders/Grid/grid.vert", "Shaders/Grid/grid.frag"});
  // clang-format on
  glCreateVertexArrays(1, &grid.get_ref<GridComponent>()->vao);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

MOD_END(Grid)