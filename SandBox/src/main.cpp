
#include <glad/glad.h>

#include <array>

#include "Components/Components.hpp"
#include "Components/Model/MeshComp.hpp"
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

// #pragma region Model
// void LoadDuck(UID &duck, Handle &texture) {
// #pragma region Model Loading
//   auto uid = TextureLoader::GetInstance().LoadAndCompile(
//       R"(data\rubber_duck\textures\Duck_baseColor.png)"
//       //	        R"(data/wall.jpg)"
//   );
//   texture = TextureLoader::GetInstance().GetHandle(uid);
//   duck = ModelLoader::GetInstance()->LoadModelAndUpLoad(
//       R"(data\rubber_duck\scene.gltf)",
//       //
//       "data\\glTF-Sample-Models\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf",
//       "PPrubberDuck");
// #pragma endregion.
// }
//
// void LoadSuit(UID &id, std::vector<Handle> &textures) {
//   id = ModelLoader::GetInstance()->LoadModelAndUpLoad(
//       R"(data\nanosuit\nanosuit.obj)", "NanoSuit");
// #d ef ine LOAD(path)                                                   \
//  {                                                                  \
//    auto uid = TextureLoader::GetInstance().LoadAndCompile(path);    \
//    textures.push_back(TextureLoader::GetInstance().GetHandle(uid)); \
//  }
//   LOAD(R"(data\nanosuit\arm_dif.png)");
//   LOAD(R"(data\nanosuit\arm_showroom_ddn.png)");
//   LOAD(R"(data\nanosuit\arm_showroom_spec.png)");
//   LOAD(R"(data\nanosuit\body_dif.png)");
//   LOAD(R"(data\nanosuit\body_showroom_ddn.png)");
//   LOAD(R"(data\nanosuit\body_showroom_spec.png)");
//   LOAD(R"(data\nanosuit\glass_ddn.png)");
//   LOAD(R"(data\nanosuit\glass_dif.png)");
//   LOAD(R"(data\nanosuit\hand_dif.png)");
//   LOAD(R"(data\nanosuit\hand_showroom_ddn.png)");
//   LOAD(R"(data\nanosuit\hand_showroom_spec.png)");
//   LOAD(R"(data\nanosuit\helmet_diff.png)");
//   LOAD(R"(data\nanosuit\helmet_showroom_ddn.png)");
//   LOAD(R"(data\nanosuit\helmet_showroom_spec.png)");
//   LOAD(R"(data\nanosuit\leg_dif.png)");
//   LOAD(R"(data\nanosuit\leg_showroom_ddn.png)");
//   LOAD(R"(data\nanosuit\leg_showroom_spec.png)");
// }
// #pragma endregion

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
          {position, glm::lookAt(position, target, up), up})
      .add<Component::InputController>();
}

// void ModelTest(world &ecs) {
//   UID duck;
//   Handle texture_handle;
//   LoadDuck(duck, texture_handle);
//
//   ecs.entity("ForwardRender")
//       .set<ShaderFile>({
//           "Shaders\\ForwardRender\\Default.vert",
//           "Shaders\\ForwardRender\\Default.frag",
//       });
//   if (duck != 0) {
//     auto name = ModelLoader::GetInstance()->GetName(duck);
//
//     // temp split them up to each single component to test
//     ecs.entity(name.c_str())
//         .set<Component::Model>(
//             {.hProgram = ecs.entity("ForwardRender").get<Program>()->handle,
//              .hVAO = ModelLoader::GetInstance()->GetVaoHandle(duck),
//              .hVertices = ModelLoader::GetInstance()->GetVboHandle(duck),
//              .hMatrices = CreateMatrices(),
//              .uIndicesSize = static_cast<unsigned int>(
//                  ModelLoader::GetInstance()->GetIndicesCount(duck))})
//         .set<Component::Texture>({.hHandle = texture_handle})
//         .set<Component::Position>({{1.0f, 1.0f, 1.0f}});
//   }
// }
//
// void PBRTest(world &ecs) {
//   auto load_and_get = [](TextureType type, const std::string &path) ->
//   Texture {
//     Handle hHandle = TextureLoader::GetInstance().GetHandle(
//         TextureLoader::GetInstance().LoadAndCompile(path));
//     auto Type = type;
//     return {hHandle, Type};
//   };
//
//   auto texalbedo =
//       load_and_get(TextureType::Albedo,
//                    "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/"
//                    "Default_albedo.jpg");
//   auto texAO = load_and_get(
//       TextureType::AmbientOcclusion,
//       "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_AO.jpg");
//   auto texEmissive = load_and_get(
//       TextureType::Emissive,
//       "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_emissive.jpg");
//   auto texMetallicRoughness =
//       load_and_get(TextureType::Metalness,
//                    "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/"
//                    "Default_metalRoughness.jpg");
//   auto texNormal = load_and_get(
//       TextureType::Normal,
//       "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_normal.jpg");
//
//   auto uid_pbr = ProgramLoader::GetInstance()->LoadAndLink(
//       {"Shaders/PBR/PBR.vert", "Shaders/PBR/PBR.frag"}, "PBR");
//
//   auto uid_model = ModelLoader::GetInstance()->LoadModelAndUpLoad(
//       R"(data\glTF-Sample-Models\2.0\DamagedHelmet\glTF\DamagedHelmet.gltf)",
//       "PBR_DamagedHelmet");
//
//   GLuint modelMatrices;
//   if (uid_model != 0) {
//     glCreateBuffers(1, &modelMatrices);
//     glNamedBufferStorage(modelMatrices, sizeof(glm::mat4), nullptr,
//                          GL_DYNAMIC_STORAGE_BIT);
//     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices);
//
//     auto name = ModelLoader::GetInstance()->GetName(uid_model);
//
//     std::array<Handle, TextureType::TextureTypeCount> textures{};
//     textures[TextureType::Albedo] = texalbedo.hHandle;
//     textures[TextureType::AmbientOcclusion] = texAO.hHandle;
//     textures[TextureType::Emissive] = texEmissive.hHandle;
//     textures[TextureType::Metalness] = texMetallicRoughness.hHandle;
//     textures[TextureType::Normal] = texNormal.hHandle;
//
//     ecs.entity(name.c_str())
//         .set<Component::Model>({
//             .hProgram = uid_pbr,
//             .hVAO = ModelLoader::GetInstance()->GetVaoHandle(uid_model),
//             .hVertices = ModelLoader::GetInstance()->GetVboHandle(uid_model),
//             .hMatrices = modelMatrices,
//             .uIndicesSize = static_cast<unsigned int>(
//                 ModelLoader::GetInstance()->GetIndicesCount(uid_model)),
//
//         })
//         .set<Component::PBRTexture>({textures})
//         .set<Component::Position>({{1.0f, 1.0f, 1.0f}})
//         .set<Component::Rotation>({{0.0f, 0.0f, 1.0f}})
//         .set<Component::Scale>({{1.0f, 1.0f, 1.0f}});
//   }
// }
//
// void PostProcessTest(world &ecs) {
//   auto blur = ecs.entity("blur").set<ShaderFile>(
//       {"Shaders\\PostProcess\\Basic.vert",
//       "Shaders\\PostProcess\\Blur.frag"});
//   auto none_effect = ecs.entity("none_effect")
//                          .set<ShaderFile>({"Shaders\\PostProcess\\Basic.vert",
//                                            "Shaders\\PostProcess\\Basic.frag"});
//   auto grayscale =
//       ecs.entity("grayscale")
//           .set<ShaderFile>({"Shaders\\PostProcess\\Basic.vert",
//                             "Shaders\\PostProcess\\Grayscale.frag"});
//   auto inversion =
//       ecs.entity("inversion")
//           .set<ShaderFile>({"Shaders\\PostProcess\\Basic.vert",
//                             "Shaders\\PostProcess\\Inversion.frag"});
//
//   //  UID uid_duck;
//   //  LoadDuck(uid_duck);
//   //  GLuint modelMatrices;
//   //  if (uid_duck != 0) {
//   //    glCreateBuffers(1, &modelMatrices);
//   //    glNamedBufferStorage(modelMatrices, 2 * sizeof(glm::mat4), nullptr,
//   //                         GL_DYNAMIC_STORAGE_BIT);
//   //    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices);
//   //    auto name = ModelLoader::GetInstance()->GetName(uid_duck);
//   //
//   //    // temp split them up to each single component to test
//   //    ecs.entity(name.c_str())
//   //        .set<Component::Model>(
//   //            {.hProgram = handle,
//   //             .hVAO = ModelLoader::GetInstance()->GetVaoHandle(uid_duck),
//   //             .hVertices =
//   //             ModelLoader::GetInstance()->GetVboHandle(uid_duck),
//   .hMatrices
//   //             = modelMatrices, .uIndicesSize = static_cast<unsigned int>(
//   //                 ModelLoader::GetInstance()->GetIndicesCount(uid_duck))})
//   //        .set<Component::Texture>({.hHandle = texture_handle})
//   //        .set<Component::Position>({{1.0f, 1.0f, 1.0f}})
//   //        .set<PostProcess>({.backupHandles = {blur.get<Program>()->handle,
//   // none_effect.get<Program>()->handle,
//   // grayscale.get<Program>()->handle,
//   // inversion.get<Program>()->handle},
//   //                           .handle = blur.get<Program>()->handle});
//   //  }
// }
// void LightTest(world &ecs) {
// #pragma region Model Loading
//   auto uid = TextureLoader::GetInstance().LoadAndCompile(
//       R"(D:\VSProject\Graphic\MonkeyRender\bin\data\glTF-Sample-Models\2.0\Box
//       With Spaces\glTF\glTF Logo With Spaces.png)"
//       //	        R"(data/wall.jpg)"
//   );
//   auto texture_handle = TextureLoader::GetInstance().GetHandle(uid);
//
//   auto uid_duck = ModelLoader::GetInstance()->LoadModelAndUpLoad(
//       R"(data\glTF-Sample-Models\2.0\Box With Spaces\glTF\Box With
//       Spaces.gltf)",
//       //
//       "data\\glTF-Sample-Models\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf",
//       //      R"(data\Container\Container.obj)",
//       "LightTestBox");
// #pragma endregion.
//
// #pragma region Light
//   auto post_handle = ProgramLoader::GetInstance()->LoadAndLink(
//       {
//           "Shaders\\PostProcess\\Basic.vert",
//           "Shaders\\PostProcess\\Blur.frag",
//       },
//       "screen_shader");
//   auto light_handle = ProgramLoader::GetInstance()->LoadAndLink(
//       {
//           "Shaders\\LightShader\\Light.vert",
//           "Shaders\\LightShader\\Light.frag",
//       },
//       "light_shader");
// #pragma endregion
//
//   ecs.entity("PointLight").add<PointLight>();
//
//   GLuint modelMatrices;
//   if (uid_duck != 0) {
//     glCreateBuffers(1, &modelMatrices);
//     glNamedBufferStorage(modelMatrices, 2 * sizeof(glm::mat4), nullptr,
//                          GL_DYNAMIC_STORAGE_BIT);
//     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices);
//     auto name = ModelLoader::GetInstance()->GetName(uid_duck);
//
//     // temp split them up to each single component to test
//     ecs.entity(name.c_str())
//         .set<Component::Model>(
//             {.hProgram = light_handle,  // light shader
//              .hVAO = ModelLoader::GetInstance()->GetVaoHandle(uid_duck),
//              .hVertices = ModelLoader::GetInstance()->GetVboHandle(uid_duck),
//              .hMatrices = modelMatrices,
//              .uIndicesSize = static_cast<unsigned int>(
//                  ModelLoader::GetInstance()->GetIndicesCount(uid_duck))})
//         .set<Component::Texture>({.hHandle = texture_handle})
//         .set<Component::Position>({{1.0f, 1.0f, 1.0f}});
//   }
// }
//
// void DefferedTest(world &ecs) {
//   ecs.entity("GeomPass")
//       .set<ShaderFile>({
//           "Shaders\\DefferedRender\\Geom.vert",
//           "Shaders\\DefferedRender\\Geom.frag",
//       });
//   ecs.entity("LightPass")
//       .set<ShaderFile>({
//           "Shaders\\DefferedRender\\LightPass.vert",
//           "Shaders\\DefferedRender\\LightPass.frag",
//       });
//   ecs.entity("ForwardRender")
//       .set<ShaderFile>({
//           "Shaders\\ForwardRender\\Default.vert",
//           "Shaders\\ForwardRender\\Default.frag",
//       });
//   UID duck;
//   Handle texture_handle;
//   LoadDuck(duck, texture_handle);
//
//   UID suit;
//   std::vector<Handle> textures;
//   LoadSuit(suit,textures);
//   if (duck != 0) {
//     ecs.entity("DefferedDuck")
//         .set<Component::Model>(
//             {.hProgram = ecs.entity("ForwardRender").get<Program>()->handle,
//              .hVAO = ModelLoader::GetInstance()->GetVaoHandle(duck),
//              .hVertices = ModelLoader::GetInstance()->GetVboHandle(duck),
//              .hMatrices = CreateMatrices(),
//              .uIndicesSize = static_cast<unsigned int>(
//                  ModelLoader::GetInstance()->GetIndicesCount(duck))})
//         .set<Component::Texture>({.hHandle = texture_handle})
//         .set<Component::Position>({{1.0f, 1.0f, 1.0f}})
//         .set<DefferedRenderer>(
//             {.geomHandle = ecs.entity("GeomPass").get<Program>()->handle,
//              .lightHandle = ecs.entity("LightPass").get<Program>()->handle});
//   }
// }

void MeshTest(world &ecs) {
  auto duck =
      ecs.entity("RubberDuck")
          .set<MeshFile>({.path = R"(data\rubber_duck\scene.gltf)"})
          .set<Texture>(
              {.path = R"(data\rubber_duck\textures\Duck_baseColor.png)"})
          .add<Transform>();

  for (int x = 0; x < 50; ++x) {
    for (int y = 0; y < 20; ++y) {
      std::string name =
          "RubberDuck_x" + std::to_string(x) + "y" + std::to_string(y);
      ecs.entity(name.c_str())
          .set<Mesh>({*duck.get<Mesh>()})
          .set<TextureHandle>({*duck.get<TextureHandle>()})
          .set<Position>({{x * 2.0f, 0.0f, y * 2.0f}})
          .add<Transform>();
    }
  }
  duck.disable();
  //  float step = 1.0f;
  //  for (int i = 0; i < 1000; ++i) {
  //    std::string name = "RubberDuck" + std::to_string(i);
  //    step = i * 10 * 0.2f;
  //
  //    ecs.entity(name.c_str())
  //        .set<Mesh>({*duck.get<Mesh>()})
  //        .set<TextureHandle>({*duck.get<TextureHandle>()})
  //        .set<Position>({{step, 0.0f, 0.0f}})
  //        .add<Transform>();
  //  }
  //  duck.disable();
  /*  ecs.entity("glTFDuck")
        .set<MeshFile>({R"(data\glTF-Sample-Models\2.0\Duck\glTF\Duck.gltf)"})
        .set<Texture>({R"(data/glTF-Sample-Models/2.0/Duck/glTF/DuckCM.png)"})
        .set<Position>({{10.0f, 0.0f, 0.0f}});*/
}
int main() {
  Core::GetInstance()->Initialize(196, 8);

  Core::GetInstance()->EnableRest();
  Core::GetInstance()
      ->Import<Module::InputModule>()  // input operation
      .Import<Module::CameraModule>()  // camera operation
      .Import<TransformModule>()
      .Import<ProgramModule>()  // shader program loader
      .Import<MeshModule>()
      .Import<TextureModule>()
      .Import<LightModule>()               // Light module , still working on it
      .Import<BaseRender>()                // Render sections
      .Import<ForwardRender>()             // Render sections
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
