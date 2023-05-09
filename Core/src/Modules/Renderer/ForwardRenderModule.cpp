// #include "ForwardRenderModule.hpp"
//
// #include <flecs.h>
// #include <glad/glad.h>
//
// #include <glm/ext.hpp>
// #include <glm/glm.hpp>
//
// #include "Base/Position.hpp"
// #include "Logger.hpp"
// #include "MeshComp.hpp"
// #include "RenderComp.hpp"
// #include "ShaderComp.hpp"
// #include "Texture.hpp"
//
//// so now I need to implement a render pass system
////  1. PerFrameData Updater
////  2. ForwardRender System
////  3. pbr bind system
////  4. update lights
////  5. update meshes
////  6. blit to screen
// using namespace glm;
// MOD_BGN(ForwardRenderModule)
//
// using namespace Component;
// std::string ForwardRender::preFab = "";
//
// void OnRenderIter(flecs::iter& it, size_t i, ForwardRenderer& renderer) {
//   glEnable(GL_DEPTH_TEST);
//   glDisable(GL_BLEND);
//
//   // here need to render to a frame buffer , then blit to screen with other
//   // render pass
//
//   auto self = it.entity(i);
//   const auto mesh = self.get<Mesh>();
//   const auto texture = self.get<TextureHandle>();
//   const auto transform = self.get<Transform>();
//
//   glUseProgram(renderer.handle);
//   // update Texture
//   {
//     if (texture) glBindTextures(0, 1, &texture->handle);
//   }
//
//   {
//     if (mesh) {
//       glBindVertexArray(mesh->vao);
//       glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh->Vertices);
//       glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, transform->Matrices);
//       glDrawElementsInstanced(GL_TRIANGLES,
//                               static_cast<GLsizei>(mesh->numIndices),
//                               GL_UNSIGNED_INT, nullptr, 1);
//       // glDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type,
//       // const void *indices,GLsizei instancecount);
//     }
//   }
//
//   glEnable(GL_BLEND);
// }
//
// void OnRenderUpdate(flecs::entity self, ForwardRenderer& renderer) {
//   glEnable(GL_DEPTH_TEST);
//   glDisable(GL_BLEND);
//   glUseProgram(renderer.handle);
//
//   FrameBuffer* frame_buffer = NULL;
//   if (self.has<FrameBuffer>()) {
//     frame_buffer = self.get_mut<FrameBuffer>();
//   }
//
//   if (frame_buffer) {
//     frame_buffer->Bind();
//
//     frame_buffer->Unbind();
//   }
//
//   glEnable(GL_BLEND);
// }
//
///// @brief for make each meshes in render unique in case the same mesh got
///// rendered multiple times
///// @param it
///// @param i
///// @param render
// void MakeRendererUnique(flecs::iter& it, size_t i, ForwardRenderer& render) {
//   auto self = it.entity(i);
//
//   if (self.has<BaseRenderer>()) {
//     Logger::get("Renderer")->warn("Found BaseRenderer, remove it");
//     self.remove<BaseRenderer>();
//   }
//   if (self.has<DefferedRenderer>()) {
//     Logger::get("Renderer")->warn("Found Deffered renderer, remove it");
//     self.remove<DefferedRenderer>();
//   }
//
//   // set up the forward render's program
//   if (render.handle == 0) {
//     auto prefab = it.world().lookup(ForwardRender::preFab.c_str());
//     self.is_a(prefab);
//   }
// }
//
// ForwardRender::ForwardRender(world& ecs) {
//   ecs.module<ForwardRender>();
//   BuildForwardPreFab(ecs);
//   LoadForwardSystem(ecs);
//
//   RegisterComponent(ecs);
//   LoadObserver(ecs);
// }
// void ForwardRender::BuildForwardPreFab(world& ecs) {
//   // due to prefab can't triggered the event to let the shader compile (just
//   // because in logic prefab does not exist even if in ecs it does exist and
//   has
//   // it's own id) so I need to create a entity to trigger the shader compile,
//   // then pass the program handle to the prefab
//   auto compiler = ecs.entity("ShaderCompiler")
//                       .set<ShaderFile>({
//                           R"(Shaders\ForwardRender\Default.vert)",
//                           R"(Shaders\ForwardRender\Default.frag)",
//                       });
//
//   preFab = ecs.prefab("ForwardDefaultProgram")
//                .set<Program>({compiler.get<Program>()->handle})
//                .path();
//
//   // now compiler has done it's job , disable it
//   compiler.disable();
// }
// void ForwardRender::LoadForwardSystem(world& ecs) {
//   // use iter to draw
//   ecs.system<ForwardRenderer>("ForwardRenderSystem")
//       .kind(flecs::OnUpdate)
//       .each(OnRenderIter);
// }
//
// void ForwardRender::LoadObserver(world& ecs) {
//   ecs.observer<ForwardRenderer>()
//       .event(flecs::OnAdd)
//       .event(flecs::OnSet)
//       .each(MakeRendererUnique);
// }
//
// void ForwardRender::RegisterComponent(world& ecs) {
//   ecs.component<ForwardRenderer>().member<Handle>("Handle").member<uint32_t>(
//       "DrawType");
// }
//
// MOD_END(ForwardRenderModule)
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

void ForwardRenderSystem(flecs::iter& it, size_t i, ForwardRenderer& render,
                         const Program& program) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // here need to render to a frame buffer , then blit to screen with other
  // render pass
  auto self = it.entity(i);
  const auto mesh = self.get<Mesh>();
  const auto texture = self.get<TextureHandle>();
  const auto transform = self.get<Transform>();

  if (i == 0) {
    glUseProgram(program.handle);
    glBindTextures(0, 1, &texture->handle);
    glBindVertexArray(mesh->vao);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh->Vertices);
  }

  //  // update Texture
  //  {
  //    if (texture)
  //  }
  //  {
  //    if (mesh) {
  //
  //      // glDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type,
  //      // const void *indices,GLsizei instancecount);
  //    }
  //  }

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, transform->Matrices);
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh->numIndices),
                          GL_UNSIGNED_INT, nullptr, 1);

  // reset to the default frame buffer
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardRenderSystemOffScreen(flecs::entity self, ForwardRenderer& render,
                                  const Program& program,
                                  const FrameBuffer& fb) {
  //  glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
  //  ForwardRenderSystem(self, render, program);
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ForwardRenderModule::ForwardRenderModule(world& ecs) {
  ecs.module<ForwardRenderModule>();
  pWorld_ = &ecs;

  CreateDefaultProgram();
  LoadComponent();
  //  [this] { this->LoadComponent(); };
  ecs.system<ForwardRenderer, const Program>("ForwardRenderSystem")
      .kind(Phase::RenderStage)
      .each(ForwardRenderSystem);
  ecs.system<ForwardRenderer, const Program, const FrameBuffer>(
         "ForwardRenderOffscreen")
      .kind(Phase::RenderStage)
      .each(ForwardRenderSystemOffScreen);
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
                  .set<FrameBuffer>({});
    isPreFabCreated_ = true;
  }
}

void ForwardRenderModule::LoadComponent() {
  pWorld_->component<ForwardRenderer>("ForwardRenderer")
      .member<uint32_t>("DrawType");

  pWorld_->observer<ForwardRenderer>("ForwardRenderInitialize")
      .event(flecs::OnAdd)
      .event(flecs::OnSet)
      .each([&](flecs::entity self, ForwardRenderer& renderer) {
        if (isPreFabCreated_)
          self.is_a(prefab_);
        else
          Logger::get<ForwardRenderModule>()->error(
              "forward render prefab does not create");
      });
}

MOD_END(ForwardRenderModule)
