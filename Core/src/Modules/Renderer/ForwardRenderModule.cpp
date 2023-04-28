#include "ForwardRenderModule.hpp"

#include <flecs.h>
#include <glad/glad.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "Base/Position.hpp"
#include "Logger.hpp"
#include "MeshComp.hpp"
#include "ShaderComp.hpp"
#include "Texture.hpp"

using namespace glm;
MOD_BGN(ForwardRenderModule)

using namespace Component;

void OnRenderIter(flecs::iter& it, size_t i, ForwardRenderer& renderer) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // here need to render to a frame buffer , then blit to screen with other
  // render pass

  auto target = it.entity(i);
  const auto mesh = target.get<Mesh>();
  const auto texture = target.get<TextureHandle>();
  const auto transform = target.get<Transform>();

  glUseProgram(renderer.handle);
  // update Texture
  {
    if (texture) glBindTextures(0, 1, &texture->handle);
  }

  {
    if (mesh) {
      glBindVertexArray(mesh->vao);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh->Vertices);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, transform->Matrices);
      glDrawElementsInstanced(GL_TRIANGLES,
                              static_cast<GLsizei>(mesh->numIndices),
                              GL_UNSIGNED_INT, nullptr, 1);
      // glDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type,
      // const void *indices,GLsizei instancecount);
    }
  }

  glEnable(GL_BLEND);
}
Handle defaulthandle = 0;
void MakeRenderUnique(entity self, ForwardRenderer& render) {
  if (self.has<BaseRenderer>()) {
    Logger::get("Renderer")->warn("Found BaseRenderer, remove it");
    self.remove<BaseRenderer>();
  }
  if (self.has<DefferedRenderer>()) {
    Logger::get("Renderer")->warn("Found Deffered renderer, remove it");
    self.remove<DefferedRenderer>();
  }

  if (render.handle == 0) {
    render.handle = defaulthandle;
  }
}

ForwardRender::ForwardRender(world& ecs) {
  ecs.module<ForwardRender>();

  // use iter to draw
  ecs.system<ForwardRenderer>("ForwardRenderSystem")
      .kind(flecs::OnUpdate)
      .each(OnRenderIter);

  auto e = ecs.entity("ForwardDefaultProgram")
               .set<ShaderFile>({
                   R"(Shaders\ForwardRender\Default.vert)",
                   R"(Shaders\ForwardRender\Default.frag)",
               });
  defaulthandle = e.get<Program>()->handle;

  ecs.observer<ForwardRenderer>()
      .event(flecs::OnAdd)
      .event(flecs::OnSet)
      .each(MakeRenderUnique);
}
MOD_END(ForwardRenderModule)

// so now I need to implement a render pass system
//  1. PerFrameData Updater
//  2. ForwardRender System
//  3. pbr bind system
//  4. update lights
//  5. update meshes
//  6. blit to screen