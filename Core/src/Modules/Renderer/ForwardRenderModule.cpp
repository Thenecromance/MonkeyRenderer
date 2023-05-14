
#include "ForwardRenderModule.hpp"

#include <glad/glad.h>

#include <Phases.hpp>

#include "Job.hpp"
#include "Logger.hpp"
#include "MeshComp.hpp"
#include "Position.hpp"
#include "RenderComp.hpp"
#include "ShaderComp.hpp"
#include "Texture.hpp"

MOD_BGN(ForwardRenderModule)
using namespace Component;

void ForwardRenderSystemIter(flecs::iter& it, ForwardRenderComp* render) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  const auto texture = it.entity(0).get<TextureHandle>();
  const auto mesh = it.entity(0).get<Mesh>();

  glUseProgram(it.entity(0).get<Program>()->handle);
  glBindTextures(0, 1, &texture->handle);
  mesh->DrawInstance(GL_TRIANGLES, it.count());
}
//
// void ForwardRenderSystemOffScreen(flecs::entity self, ForwardRenderComp&
// render,
//                                  const Program& program,
//                                  const FrameBuffer& fb) {
//  //  glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
//  //  ForwardRenderSystem(self, render, program);
//  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}

ForwardRenderModule::ForwardRenderModule(world& ecs) {
  ecs.module<ForwardRenderModule>();
  pWorld_ = &ecs;

  CreateDefaultProgram();
  LoadComponent();
  //  [this] { this->LoadComponent(); };
  ecs.system<ForwardRenderComp>("ForwardRenderSystem")
      .kind(Phase::RenderStage)
      .iter(ForwardRenderSystemIter);
  /*
    ecs.system<ForwardRenderComp, const Program, const FrameBuffer>(
           "ForwardRenderOffscreen")
        .kind(Phase::RenderStage)
        .each(ForwardRenderSystemOffScreen);*/
}

void ForwardRenderModule::CreateDefaultProgram() {
  auto compiler = pWorld_->entity("ForwardDefaultShaderCompiler")
                      .set<ShaderFile>({
                          R"(Shaders\ForwardRender\Default.vert)",
                          R"(Shaders\ForwardRender\Default.frag)",
                      });  // this can be use as shader hot reload module
  if (compiler.has<Program>()) {
    auto program = compiler.get<Program>();
    if (program->handle == 0) {
      Logger::get<ForwardRenderModule>()->critical(
          "ForwardRender default program compile failed");
    }

    prefab_ = pWorld_->prefab("DefaultForwardRenderer")
                  .set<Program>({program->handle})
                  .add<FrameBuffer>();
  }
}

void ForwardRenderModule::LoadComponent() {
  pWorld_->component<ForwardRenderComp>()
      .member<uint32_t>("DrawType");

  pWorld_->observer<ForwardRenderComp>("ForwardRenderInitialize")
      .event(flecs::OnAdd)
      //      .event(flecs::OnSet)
      .each([&](flecs::entity self, ForwardRenderComp& renderer) {
        if (prefab_.is_alive())  // prefab is also an entity, so just use
                                 // is_alive() to check it's create or not
        {
          self.is_a(prefab_);
        } else {
          Logger::get<ForwardRenderModule>()->error(
              "forward render prefab does not create");
        }
      });
}

MOD_END(ForwardRenderModule)
