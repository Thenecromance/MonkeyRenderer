// #include "RenderModule.hpp"

// #include <flecs.h>
// #include <glad/glad.h>

// #include "Logger.hpp"
// #include "RenderComponents.hpp"

// using namespace Monkey::Component;

// MOD_BGN(Render)
// unsigned int vao;

// void OnBaseRenderUpdate(RenderComponents &render) {
//   NONE_ZERO_CHECK(render.ProgramHandle);

//   glDisable(GL_BLEND);
//   glBindVertexArray(vao);
//   glUseProgram(render.ProgramHandle);

//   glDrawArrays(GL_TRIANGLES, 0, render.iIndicesCount);
//   glEnable(GL_BLEND);
// }

// RenderModule::RenderModule(world &ecs) {
//   glCreateVertexArrays(1, &vao);
//   glBindVertexArray(vao);
//   ecs.module<RenderModule>();

//   Logger::get("Module")->trace("{} Loaded", __FUNCTION__);

//   ecs.component<RenderComponents>()
//       .member<Handle>("ProgramHandle")
//       .member<unsigned int>("IndicesCount");

//   ecs.system<RenderComponents>("BasicRenderSystem")
//       .kind(flecs::OnStore)
//       .each(OnBaseRenderUpdate);
// }
// MOD_END(Render)