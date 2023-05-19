
#include "ForwardRenderModule.hpp"

#include <glad/glad.h>

#include <Phases.hpp>

#include "GlobalValue.hpp"
#include "Job.hpp"
#include "Logger.hpp"
#include "MeshComp.hpp"
#include "Position.hpp"
#include "RenderComp.hpp"
#include "ShaderComp.hpp"
#include "Texture.hpp"

MOD_BGN(ForwardRenderModule)
using namespace Component;

void ForwardRenderSystemIter(flecs::iter& it, ForwardRenderComp* render,
                             Mesh* meshes) {
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  auto group = it.entity(0).get<TransformGroup>();
  auto self = it.entity(0);

  glUseProgram(it.entity(0).get<Program>()->handle);
  
  glBindTextureUnit(0, it.entity(0).get<TextureHandle>()->handle);
  
  
  
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER,Uniform::BindingLocation::eMatrices, group->groupedHandle);
  glNamedBufferSubData(group->groupedHandle, 0, sizeof(glm::mat4) * it.count(),self.get<Transform>());
  meshes[0].DrawInstance(render[0].drawType, it.count());
}

ForwardRenderModule::ForwardRenderModule(world& ecs) {
  ecs.module<ForwardRenderModule>();
  pWorld_ = &ecs;

  CreateDefaultProgram();
  LoadComponent();
  //  [this] { this->LoadComponent(); };
  ecs.system<ForwardRenderComp, Mesh>("ForwardRenderSystem")
      .kind(Phase::RenderStage)
      .iter(ForwardRenderSystemIter);
  /*//off screen render system
    ecs.system<ForwardRenderComp, const Program, const FrameBuffer>(
           "ForwardRenderOffscreen")
        .kind(Phase::RenderStage)
        .each(ForwardRenderSystemOffScreen);*/
}

void ForwardRenderModule::CreateDefaultProgram() {
  defaultProgram_ = pWorld_->entity("ForwardDefaultShaderCompiler")
                        .set<ShaderFile>({
                            R"(Shaders\ForwardRender\Default.vert)",
                            R"(Shaders\ForwardRender\Default.frag)",
                        });  // this can be use as shader hot reload module

  auto program = defaultProgram_.get<Program>();
  if (program->handle == 0) {
    Logger::get<ForwardRenderModule>()->critical(
        "ForwardRender default program compile failed");
  }

  prefab_ = pWorld_->prefab("DefaultForwardRenderer")
                .set<Program>({program->handle})
                .add<FrameBuffer>();
}

void ForwardRenderModule::LoadComponent() {
  pWorld_->component<ForwardRenderComp>().member<uint32_t>("DrawType");

  pWorld_->observer<ForwardRenderComp>("ForwardRenderInitialize")
      .event(flecs::OnAdd)
      //      .event(flecs::OnSet)
      .each([&](flecs::entity self, ForwardRenderComp& renderer) {
        // if user does not set the shader program, so just use the default one
        Handle program_handle = 0;
        if (self.has<Program>()) {
          program_handle = self.get<Program>()->handle;
        }

        if (prefab_.is_alive()) {
          self.is_a(prefab_);
        } else {
          Logger::get<ForwardRenderModule>()->error(
              "ForwardRenderModule prefab is not created");
        }
      });
}

MOD_END(ForwardRenderModule)
