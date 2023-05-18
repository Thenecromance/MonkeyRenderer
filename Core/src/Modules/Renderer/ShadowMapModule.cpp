#include "ShadowMapModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <imgui.h>

#include <glm/ext.hpp>

#include "GlobalValue.hpp"
#include "LightComp.hpp"
#include "Logger.hpp"
#include "MeshComp.hpp"
#include "OpenGLApp.hpp"
#include "Phases.hpp"
#include "RenderComp.hpp"
#include "ShaderComp.hpp"

COMP_BGN(RenderComp)
void ShadowMap::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, handle);
  glViewport(0, 0, width, height);
}
void ShadowMap::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void SSAO::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, handle);
  glViewport(0, 0, width, height);
}
void SSAO::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

COMP_END(RenderComp)

MOD_BGN(ShadowMapping)
/*
In this module : need Tag::ShadowMap, Component::ShadowMap,
Component::LightTransform, Component::Mesh
*/

/*
struct ShadowMap {
  Handle handle{};

  Handle colorHandle{};
  Handle depthHandle{};

  int height{2560};
  int width{1440};

 public:
  void Bind() const;
  static void Unbind();
};
*/

ShadowMapModule::ShadowMapModule(world& ecs) {
  ecs.module<ShadowMapModule>();
  pWorld_ = &ecs;
  shadowMapEntity_ = ecs.entity("shadowMapEntity_");

  InitializeShadowMapSystem();
  HookOtherComponents();
  InitializeShadowMapComponent();
  LoadShadowMapShader();
  InitializeLightTransformComponent();
}

void ShadowMapModule::HookOtherComponents() {
  PTR_ASSERT(pWorld_);
  // now add a shadow map tag to entity to let mesh will be rendered to shadow
  // map
  pWorld_->observer<Component::Mesh>("AddShadowMapTagToEntity")
      .event(flecs::OnAdd)
      .each([](flecs::iter& it, size_t i, Component::Mesh& mesh) {
        // to prevent tag will be added twice when other place calling the
        // entity.set<Mesh>();
        if (it.event() == flecs::OnAdd) it.entity(i).add<Tag::ShadowMap>();
      });

  pWorld_->observer<Component::PointLight>("AddLightTransformToLight")
      .event(flecs::OnAdd)
      .each([](flecs::iter& it, size_t i, Component::PointLight& light) {
        if (it.event() == flecs::OnAdd)
          it.entity(i).add<Component::LightTransform>();
      });
}

void ShadowMapModule::InitializeShadowMapComponent() {
  PTR_ASSERT(pWorld_);
  // Initialize shadow map component
  pWorld_->observer<Component::ShadowMap>("ShadowMapInitializer")
      .event(flecs::OnAdd)
      .each([](flecs::entity target, Component::ShadowMap& shadowMap) {
        
        glCreateFramebuffers(1, &shadowMap.handle);
        OpenGLApp::GetInstance()->GetWindowSize(shadowMap.width,
                                                shadowMap.height);

        // clang-format off
        // create color buffer
        {
          glCreateTextures(GL_TEXTURE_2D, 1, &shadowMap.colorHandle);
          glTextureParameteri(shadowMap.colorHandle, GL_TEXTURE_MAX_LEVEL, 0);
          glTextureParameteri(shadowMap.colorHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTextureParameteri(shadowMap.colorHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTextureStorage2D( shadowMap.colorHandle, 1, GL_RGBA8,shadowMap.width, shadowMap.height);

          glTextureParameteri(shadowMap.colorHandle, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
          glTextureParameteri(shadowMap.colorHandle, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
          glNamedFramebufferTexture(shadowMap.handle, GL_COLOR_ATTACHMENT0,shadowMap.colorHandle, 0);
        }

        // create depth buffer
        {
          glCreateTextures(GL_TEXTURE_2D, 1, &shadowMap.depthHandle);
          glTextureParameteri(shadowMap.depthHandle, GL_TEXTURE_MAX_LEVEL, 0);
          glTextureParameteri(shadowMap.depthHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTextureParameteri(shadowMap.depthHandle, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
          glTextureStorage2D(shadowMap.depthHandle, 1, GL_DEPTH_COMPONENT24,shadowMap.width, shadowMap.height);

          const GLfloat border[] = {0.0f, 0.0f, 0.0f, 0.0f};
          glTextureParameterfv(shadowMap.depthHandle, GL_TEXTURE_BORDER_COLOR,border);
          glTextureParameteri(shadowMap.depthHandle, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
          glTextureParameteri(shadowMap.depthHandle, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
          glNamedFramebufferTexture(shadowMap.handle, GL_DEPTH_ATTACHMENT,shadowMap.depthHandle, 0);
        }
        // clang-format on
        const GLenum status = glCheckNamedFramebufferStatus(shadowMap.handle, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
          Logger::get<ShadowMapModule>()->error(
              "ShadowMap Framebuffer is not complete {}", status); //36054 GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
          assert(false && "ShadowMap Framebuffer is not complete");
        }
      });
}

void ShadowMapModule::InitializeShadowMapSystem() {
  PTR_ASSERT(pWorld_);
  // this pass should be at the front of the RenderStage
  pWorld_->system<Component::Mesh, Tag::ShadowMap>("ShadowMap Pass")
      .iter([&](flecs::iter& it, Component::Mesh* meshes, Tag::ShadowMap*) {
        if(!shadowMapEntity_.has<Component::Program>()){
          Logger::get<ShadowMapModule>()->error("ShadowMap Pass need a shader program");
          return ;
        }
        glEnable(GL_DEPTH_TEST);
        
        auto shadowMap = shadowMapEntity_.get<Component::ShadowMap>();
        shadowMap->Bind();
        glUseProgram(shadowMapEntity_.get<Component::Program>()->handle);
        glClearNamedFramebufferfv(
            shadowMap->handle, GL_COLOR, 0,
            glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
        glClearNamedFramebufferfi(shadowMap->handle, GL_DEPTH_STENCIL, 0, 1.0f,
                                  0);
        for (auto idx : it) {
          meshes[idx].DrawBaseInstance(GL_TRIANGLES, 1, idx);
        }
       
        shadowMap->Unbind();
        
        glDisable(GL_DEPTH_TEST);
      });
  
  pWorld_->system<Component::ShadowMap>("UpdateLightTransform")
      .kind(Phase::ImGuiRender)
      .each([&](Component::ShadowMap& shadowMap){
        ImGui::Begin("ShadowMapPreview");
        ImGui::Image((void*)(intptr_t)shadowMap.colorHandle, ImVec2(256, 256),
                     ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Image((void*)(intptr_t)shadowMap.depthHandle, ImVec2(256, 256),
                     ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
      });
}

void ShadowMapModule::LoadShadowMapShader() {
  PTR_ASSERT(pWorld_);
  if (!shadowMapEntity_.is_alive()) {
    return;
  }
  // Load ShadowMap shader
  shadowMapEntity_
      .set<Component::ShaderFile>(
      {"Shaders//ShadowMapping//shadow.vert",
       "Shaders//ShadowMapping//shadow.frag"})
      .add<Component::ShadowMap>();
}

void UpdatePointLight(Component::PointLight& light,
                      Component::LightTransform& transform) {
  
                        const glm::mat4 lightProj = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 20.f);
                        const glm::mat4 lightView = glm::lookAt(glm::vec3(light.position), glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0, 1, 0));
  
                        transform.projection = lightProj;
                        transform.view = lightView;
                      }

void ShadowMapModule::InitializeLightTransformComponent() {
  PTR_ASSERT(pWorld_);

  {
    glCreateBuffers(1, &PointLightGroup);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, PointLightGroup);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 static_cast<GLsizeiptr>(Component::sizeLightTransform * 100), // temp to use 100 lights, need to change later
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,Uniform::BindingLocation::ePointLightMatrices, PointLightGroup);
    Logger::get<ShadowMapModule>()->info("PointLightGroup {}", PointLightGroup);
  }

  //update and upload light transform matrices to gpu
  pWorld_
      ->system<Component::PointLight, Component::LightTransform>(
          "PointLightTransformUpdate")
      .kind(Phase::LogicUpdate)//another wrong place
      .iter([&](flecs::iter& it, Component::PointLight* lights,
                Component::LightTransform* transforms) {
        for (auto idx : it) {
          UpdatePointLight(lights[idx], transforms[idx]);
        }

        glNamedBufferSubData(PointLightGroup, Uniform::BindingLocation::ePointLightMatrices,Component::sizeLightTransform * it.count(), transforms);
      });
}

MOD_END(ShadowMapping)
