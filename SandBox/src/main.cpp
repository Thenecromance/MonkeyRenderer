
#include <glad/glad.h>

#include <array>

#include "Components/Components.hpp"
#include "Components/Model/MeshComp.hpp"
#include "Components/Renderer/RenderComp.hpp"
#include "Components/ShaderComp.hpp"
#include "Components/Textures/Texture.hpp"
#include "Core.hpp"
#include "Modules/Importer/MeshModule.hpp"
#include "Modules/Importer/ProgramModule.hpp"
#include "Modules/Importer/TextureModule.hpp"
#include "Modules/Modules.hpp"
#include "Modules/Renderer/AntiAliasingConfigModule.hpp"
#include "Modules/Renderer/BaseRenederModule.hpp"
#include "Modules/Renderer/DefferedRender.hpp"
#include "Modules/Renderer/ForwardRenderModule.hpp"
#include "Modules/Renderer/LightModule.hpp"
#include "Modules/Renderer/PostProcessModule.hpp"
#include "Modules/TransformModule.hpp"

using namespace Monkey;
using namespace Monkey::Component;
using namespace Monkey::Module;

using namespace glm;

Handle CreateMatrices() {
  GLuint modelMatrices;
  glCreateBuffers(1, &modelMatrices);
  glNamedBufferStorage(modelMatrices, 2 * sizeof(glm::mat4), nullptr,
                       GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices);
  return modelMatrices;
}
void CreateCamera(world &ecs) {
  glm::vec3 position = glm::vec3(0.0f, 10.0f, 0.0f);
  glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  ecs.entity("MainCamera")
      .set<Component::CameraComponent>(
          {.v3Position = {position},
           .Orientation = glm::lookAt(position, target, up),
           .up = up,
           .isActiveCamera = true})
      .add<Component::InputController>();
}

void MeshTest(world &ecs) {
  auto duck =
      ecs.entity("RubberDuck")
          .set<MeshFile>({.path = R"(data\rubber_duck\scene.gltf)"})
          .set<Texture>(
              {.path = R"(data\rubber_duck\textures\Duck_baseColor.png)"})
          .add<Transform>()
          .add<DefferedRenderComp>()
          .disable<ForwardRenderComp>();

  // when using the core::EnableRest() , a large multi draw will spend a
  // lot of time on the CPU to upload the data to dashboard , caused the low fps
  for (int x = 0; x < 50; ++x) {
    for (int y = 0; y < 50; ++y) {
      std::string name =
          "RubberDuck_x" + std::to_string(x) + "y" + std::to_string(y);

      ecs.entity(name.c_str())
          .set<Mesh>({*duck.get<Mesh>()})
          .set<TextureHandle>({*duck.get<TextureHandle>()})
          .set<Position>({{x * 2.0f, 0.0f, y * 2.0f}})
          .add<Transform>()
          .disable<ForwardRenderComp>()
          .add<DefferedRenderComp>();
    }
  }
  //   duck.disable();
}
int main() {
  Core::GetInstance()->Initialize(196, 8);

  Core::GetInstance()->EnableRest();
  Core::GetInstance()
      ->Import<Module::InputModule>()  // input operation
      .Import<Module::CameraModule>()  // camera operation
      .Import<TransformModule>()
      .Import<ProgramModule>()          // shader program loader
      .Import<ShaderHotReloadModule>()  // shader hot reload module
      .Import<MeshModule>()
      .Import<TextureModule>()
      .Import<LightModule>()               // Light module , still working on it
      .Import<BaseRender>()                // Render sections
      .Import<ForwardRenderModule>()       // Render sections
      .Import<DefferedRender>()            // Render sections
      .Import<Module::GridModule>()        // grid
      .Import<Module::ImGuiRenderer>()     // ImGuiRenderer
      .Import<AntiAliasingConfigModule>()  // AA control
      ;

  CreateCamera(Core::GetInstance()->GetWorld());
  MeshTest(Core::GetInstance()->GetWorld());
  while (Core::GetInstance()->OnUpdate())
    ;

  Core::GetInstance()->Release();

  return 0;
}
