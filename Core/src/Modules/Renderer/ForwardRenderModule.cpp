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

void OnRenderIter(flecs::iter& it) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  for (auto i : it) {
    auto target = it.entity(i);

    const auto mesh = target.get<Mesh>();
    const auto texture = target.get<TextureHandle>();
    const auto transform = target.get<Transform>();
    const auto renderer = target.get<ForwardRenderer>();

    glUseProgram(renderer->handle);
    // update Texture
    {
      if (texture) glBindTextures(0, 1, &texture->handle);
    }

    {
      if (mesh) {
        glBindVertexArray(mesh->vao);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mesh->Vertices);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, transform->Matrices);

        glDrawElementsInstancedBaseInstance(
            GL_TRIANGLES, static_cast<GLsizei>(mesh->numIndices),
            GL_UNSIGNED_INT, nullptr, 1, 0);

        //        glDrawElementsInstancedBaseVertexBaseInstance(
        //            GL_TRIANGLES, static_cast<GLsizei>(mesh->numIndices),
        //            GL_UNSIGNED_INT, nullptr, 1, 0, 0);
      }
    }
  }

  glEnable(GL_BLEND);
}
Handle defaulthandle = 0;
void RemoveOtherRenderer(entity self, ForwardRenderer& render) {
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
      .iter(OnRenderIter);

  auto e = ecs.entity("ForwardDefaultProgram")
               .set<ShaderFile>({
                   R"(Shaders\ForwardRender\Default.vert)",
                   R"(Shaders\ForwardRender\Default.frag)",
               });
  defaulthandle = e.get<Program>()->handle;

  ecs.observer<ForwardRenderer>()
      .event(flecs::OnAdd)
      .event(flecs::OnSet)
      .each(RemoveOtherRenderer);
}
MOD_END(ForwardRenderModule)