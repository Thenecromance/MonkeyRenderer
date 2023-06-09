
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
#include "Modules/Renderer/ShadowMapModule.hpp"
#include "Modules/TransformModule.hpp"

using namespace Monkey;
using namespace Monkey::Component;
using namespace Monkey::Module;

using namespace glm;

void CreateCamera(world &ecs) {
  glm::vec3 position = glm::vec3(0.0f, 10.0f, 0.0f);
  glm::vec3 target = glm::vec3(0.0f, 10.0f, -1.0f);  // look at -z
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);        // up is +y
  ecs.entity("MainCamera")
      .set<Component::CameraComponent>(
          {.v3Position = {position},
           .Orientation = glm::lookAt(position, target, up),
           .up = up,
           .isActiveCamera = true})
      .add<Component::InputController>();
}
void PostProcessTest(world &ecs) {
  ecs.entity("screen_shader")
      .set<ShaderFile>({"Shaders\\PostProcess\\Basic.vert",
                        "Shaders\\PostProcess\\Basic.frag"});
  ecs.entity("Grayscale")
      .set<ShaderFile>({"Shaders\\PostProcess\\Basic.vert",
                        "Shaders\\PostProcess\\Grayscale.frag"});
  auto e = ecs.entity("Grayscale")
               .set<ShaderFile>({
                   "Shaders\\PostProcess\\Basic.vert",
                   "Shaders\\PostProcess\\Inversion.frag",
               });
}

void AddLights(world &ecs) {
  // clang-format off
    ecs.entity("LightGroup::PointLight0").set<Component::PointLight>({.intensity = 1.0f ,.position = {-0.0f, 2.5f,   0.0f}        ,.color =     {1.0f, 1.0f, 1.0f}     }); //temp set to positions.z to 0 for debug
    ecs.entity("LightGroup::PointLight1").set<Component::PointLight>({.intensity = 1.0f ,.position = {-10.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });
    ecs.entity("LightGroup::PointLight2").set<Component::PointLight>({.intensity = 1.0f ,.position = {-20.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });
    ecs.entity("LightGroup::PointLight3").set<Component::PointLight>({.intensity = 1.0f ,.position = {-30.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });
    ecs.entity("LightGroup::PointLight4").set<Component::PointLight>({.intensity = 1.0f ,.position = {-40.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });
    ecs.entity("LightGroup::PointLight5").set<Component::PointLight>({.intensity = 1.0f ,.position = {-50.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });
    ecs.entity("LightGroup::PointLight6").set<Component::PointLight>({.intensity = 1.0f ,.position = {-60.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });
    ecs.entity("LightGroup::PointLight7").set<Component::PointLight>({.intensity = 1.0f ,.position = {-70.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });
    ecs.entity("LightGroup::PointLight8").set<Component::PointLight>({.intensity = 1.0f ,.position = {-80.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });
    ecs.entity("LightGroup::PointLight9").set<Component::PointLight>({.intensity = 1.0f ,.position = {-90.0f, 2.5f,  50.0f}       ,.color =     {1.0f, 1.0f, 1.0f}     });

    /*
    ecs.entity("LightGroup::DirectionalLight0").add<Component::DirectionalLight>();
    ecs.entity("LightGroup::DirectionalLight1").add<Component::DirectionalLight>();
    ecs.entity("LightGroup::DirectionalLight2").add<Component::DirectionalLight>();
    ecs.entity("LightGroup::DirectionalLight3").add<Component::DirectionalLight>();
    ecs.entity("LightGroup::DirectionalLight4").add<Component::DirectionalLight>();

    ecs.entity("LightGroup::SpotLight0").add<Component::SpotLight>();
    ecs.entity("LightGroup::SpotLight1").add<Component::SpotLight>();
    ecs.entity("LightGroup::SpotLight2").add<Component::SpotLight>();
    ecs.entity("LightGroup::SpotLight3").add<Component::SpotLight>();
    ecs.entity("LightGroup::SpotLight4").add<Component::SpotLight>();*/

  // clang-format on
}
void MeshTest(world &ecs) {
  auto duck =
      ecs.entity("MeshGroup::RubberDuckBase")
          .set<MeshFile>({.path = R"(data\rubber_duck\scene.gltf)"})
          .set<Texture>(
              {.path = R"(data\rubber_duck\textures\Duck_baseColor.png)"})
          .add<Transform>();

  /*  .add<DefferedRenderComp>()
    .disable<ForwardRenderComp>()*/
  ;
  //  ecs.entity("Container")
  //          .set<MeshFile>({
  //              .path = R"(data/Container/Container.obj)"
  //          })
  //          .set<Texture>({
  //              .path = R"(data/test.jpg)"
  //          })
  //          .add<Transform>()
  //              .set<Scale>({
  //              .value = {
  //                  0.01f,0.01f,0.01f
  //              }
  //          });
  // TODO: I need to create a place to place these basic shapes
  ecs.entity("Plane")
      .set<MeshData>(
          {.Indices = {0, 1, 2, 2, 3, 0},
           .Vertex = {-2, -2, 0, 0, 0, 1, 0, 0, -2, +2, 0, 0, 0, 1, 0, 1,
                      +2, +2, 0, 0, 0, 1, 1, 1, +2, -2, 0, 0, 0, 1, 1, 0}

          })
      .set<Texture>({"data//wall.jpg"})
      .add<Transform>()
      .set<Position>({.value = {0.0f, 0.01f, 0.00f}})
      .set<Rotation>({.value = {0.0f, 0.0f, 0.0f}})
      .set<Scale>({.value = glm::vec3(10.0f)});

  // on my hard ware (RTX 2080, with 9700k) , the fps is 8~10 when direct render
  // 50K duck.

  // when using the core::EnableRest() , a large multi draw will spend a
  // lot of time on the CPU to upload the data to dashboard , caused the low fps
  for (int x = 0; x < 5; ++x) {
    for (int z = 0; z < 5; ++z) {
      std::string name = "MeshGroup::CopyMesh::RubberDuck_x" +
                         std::to_string(x) + "z" + std::to_string(z);
      auto e = ecs.lookup("::Grayscale");
      ecs.entity(name.c_str())
          .set<Mesh>({*duck.get<Mesh>()})
          .set<TextureHandle>({*duck.get<TextureHandle>()})
          .set<Position>({{x * 2.0f, 0.0f, z * 2.0f}})
          .add<Transform>()
          //          .remove<ForwardRenderComp>()
          //          .add<DefferedRenderComp>()
          ;
    }
  }
  duck.disable();
}
int main() {
  Core::GetInstance()->Initialize(144, 1);

  Core::GetInstance()->EnableRest();

  Core::GetInstance()
      ->Import<Module::InputModule>()  // input operation
      .Import<Module::CameraModule>()  // camera operation
      .Import<Module::TransformModule>()
      .Import<Module::ProgramModule>()          // shader program loader
      .Import<Module::ShaderHotReloadModule>()  // shader hot reload module
      .Import<Module::MeshModule>()
      .Import<Module::TextureModule>()
      .Import<Module::LightModule>()  // Light module , still working on it
      .Import<Module::ShadowMapModule>()
      .Import<Module::BaseRender>()           // Render sections
      .Import<Module::ForwardRenderModule>()  // Render sections
      .Import<Module::DefferedRender>()       // Render sections
      .Import<Module::GridModule>()           // grid
      .Import<Module::PostProcessModule>()
      .Import<Module::ImGuiRenderer>()             // ImGuiRenderer
      .Import<Module::AntiAliasingConfigModule>()  // AA control
      ;
  PostProcessTest(Core::GetInstance()->GetWorld());
  CreateCamera(Core::GetInstance()->GetWorld());
  MeshTest(Core::GetInstance()->GetWorld());
  AddLights(Core::GetInstance()->GetWorld());

  while (Core::GetInstance()->OnUpdate())
    ;

  Core::GetInstance()->Release();

  return 0;
}
