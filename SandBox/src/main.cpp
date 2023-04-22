
#include <glad/glad.h>

#include <array>

#include "Components/Components.hpp"
#include "Components/RenderComponents.hpp"
#include "Components/ShaderComp.hpp"
#include "Core.hpp"
#include "Modules/Importer/ModelImporter.hpp"
#include "Modules/Importer/ProgramImporter.hpp"
#include "Modules/Modules.hpp"
#include "Modules/Renderer/AntiAliasingConfigModule.hpp"
#include "Modules/Renderer/BaseRenederModule.hpp"
#include "Modules/Renderer/DefferedRender.hpp"
#include "Modules/Renderer/ForwardRenderModule.hpp"
#include "Modules/Renderer/LightModule.hpp"
#include "Modules/Renderer/PostProcessModule.hpp"
#include "Resources/MeshLoader.hpp"
#include "Resources/ShaderLoader.hpp"
#include "Resources/TextureLoader.hpp"
using namespace Monkey;
using namespace Monkey::Component;
using namespace Monkey::Module;

using namespace glm;

void CreateCamera(world &ecs) {
  glm::vec3 position = glm::vec3(0.0f, 10.0f, 0.0f);
  glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  ecs.entity("CameraTest")
      .set<Component::CameraComponent>(
          {position, glm::lookAt(position, target, up), up})
      .add<Component::InputController>();
}
// void TestShader(world &ecs) {
//   auto uidDemoVs =
//       ShaderLoader::GetInstance()->LoadAndCompile(R"(Shaders\demo.vs)");
//   auto uidDemoFs =
//       ShaderLoader::GetInstance()->LoadAndCompile(R"(Shaders\demo.fs)");
//   auto handleDemoVs = ShaderLoader::GetInstance()->GetHandle(uidDemoVs);
//   auto handleDemoFs = ShaderLoader::GetInstance()->GetHandle(uidDemoFs);
//
//   auto uidProgram = ProgramLoader::GetInstance()->LoadAndLink(
//       {handleDemoVs, handleDemoFs}, "demo");
//
//   ecs.entity("ShaderTest")
//       .set<Component::RenderComponents>(
//           {ProgramLoader::GetInstance()->GetHandle(uidProgram)});
// }
//
// void TextureLoaderTest(world &ecs) {
//   TextureLoader loader;
//
//   auto uidTexture = loader.LoadAndCompile(R"(data/wall.jpg)");
//   auto handleTexture = loader.GetHandle(uidTexture);
//
//   glBindTextures(1, 1, &handleTexture);
//
//   auto handleTextureProgram = ProgramLoader::GetInstance()->LoadAndLink(
//       {"Shaders/TextureTest/Texture.vs", "Shaders/TextureTest/Texture.fs"},
//       "texture");
//
//   ecs.entity("TextureTest")
//       .set<Component::RenderComponents>({handleTextureProgram});
// }

void ModelTest(world &ecs) {
  auto uid = TextureLoader::GetInstance().LoadAndCompile(
      R"(data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_albedo.jpg)"
      //	        R"(data/wall.jpg)"
  );
  auto texture_handle = TextureLoader::GetInstance().GetHandle(uid);

  auto uid_duck = ModelLoader::GetInstance()->LoadModelAndUpLoad(
      R"(data\Container\Container.obj)",
      //      "data\\glTF-Sample-Models\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf",
      "rubberDuck");

  auto handle = ProgramLoader::GetInstance()->LoadAndLink(
      {"Shaders/rubber_duck/rubber.vs", "Shaders/rubber_duck/rubber.fs"},
      "rubber");
  GLuint modelMatrices;
  if (uid_duck != 0) {
    glCreateBuffers(1, &modelMatrices);
    glNamedBufferStorage(modelMatrices, 2 * sizeof(glm::mat4), nullptr,
                         GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices);
    auto name = ModelLoader::GetInstance()->GetName(uid_duck);

    // temp split them up to each single component to test
    ecs.entity(name.c_str())
        .set<Component::Model>(
            {.hProgram = handle,
             .hVAO = ModelLoader::GetInstance()->GetVaoHandle(uid_duck),
             .hVertices = ModelLoader::GetInstance()->GetVboHandle(uid_duck),
             .hMatrices = modelMatrices,
             .uIndicesSize = static_cast<unsigned int>(
                 ModelLoader::GetInstance()->GetIndicesCount(uid_duck))})
        .set<Component::Texture>({.hHandle = texture_handle})
        .set<Component::Position>({{1.0f, 1.0f, 1.0f}});
  }
}

void PBRTest(world &ecs) {
  auto load_and_get = [](TextureType type, const std::string &path) -> Texture {
    Handle hHandle = TextureLoader::GetInstance().GetHandle(
        TextureLoader::GetInstance().LoadAndCompile(path));
    auto Type = type;
    return {hHandle, Type};
  };

  auto texalbedo =
      load_and_get(TextureType::Albedo,
                   "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/"
                   "Default_albedo.jpg");
  auto texAO = load_and_get(
      TextureType::AmbientOcclusion,
      "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_AO.jpg");
  auto texEmissive = load_and_get(
      TextureType::Emissive,
      "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_emissive.jpg");
  auto texMetallicRoughness =
      load_and_get(TextureType::Metalness,
                   "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/"
                   "Default_metalRoughness.jpg");
  auto texNormal = load_and_get(
      TextureType::Normal,
      "data/glTF-Sample-Models/2.0/DamagedHelmet/glTF/Default_normal.jpg");

  auto uid_pbr = ProgramLoader::GetInstance()->LoadAndLink(
      {"Shaders/PBR/PBR.vert", "Shaders/PBR/PBR.frag"}, "PBR");

  auto uid_model = ModelLoader::GetInstance()->LoadModelAndUpLoad(
      R"(data\glTF-Sample-Models\2.0\DamagedHelmet\glTF\DamagedHelmet.gltf)",
      "PBR_DamagedHelmet");

  GLuint modelMatrices;
  if (uid_model != 0) {
    glCreateBuffers(1, &modelMatrices);
    glNamedBufferStorage(modelMatrices, sizeof(glm::mat4), nullptr,
                         GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices);

    auto name = ModelLoader::GetInstance()->GetName(uid_model);

    std::array<Handle, TextureType::TextureTypeCount> textures{};
    textures[TextureType::Albedo] = texalbedo.hHandle;
    textures[TextureType::AmbientOcclusion] = texAO.hHandle;
    textures[TextureType::Emissive] = texEmissive.hHandle;
    textures[TextureType::Metalness] = texMetallicRoughness.hHandle;
    textures[TextureType::Normal] = texNormal.hHandle;

    ecs.entity(name.c_str())
        .set<Component::Model>({
            .hProgram = uid_pbr,
            .hVAO = ModelLoader::GetInstance()->GetVaoHandle(uid_model),
            .hVertices = ModelLoader::GetInstance()->GetVboHandle(uid_model),
            .hMatrices = modelMatrices,
            .uIndicesSize = static_cast<unsigned int>(
                ModelLoader::GetInstance()->GetIndicesCount(uid_model)),

        })
        .set<Component::PBRTexture>({textures})
        .set<Component::Position>({{1.0f, 1.0f, 1.0f}})
        .set<Component::Rotation>({{0.0f, 0.0f, 1.0f}})
        .set<Component::Scale>({{1.0f, 1.0f, 1.0f}});
  }
}

void PostProcessTest(world &ecs) {
#pragma region Model Loading
  auto uid = TextureLoader::GetInstance().LoadAndCompile(
      R"(data\rubber_duck\textures\Duck_baseColor.png)"
      //	        R"(data/wall.jpg)"
  );
  auto texture_handle = TextureLoader::GetInstance().GetHandle(uid);

  auto handle = ProgramLoader::GetInstance()->LoadAndLink(
      {"Shaders/rubber_duck/rubber.vs", "Shaders/rubber_duck/rubber.fs"},
      "rubber");

  auto uid_duck = ModelLoader::GetInstance()->LoadModelAndUpLoad(
      R"(data\rubber_duck\scene.gltf)",
      //      "data\\glTF-Sample-Models\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf",
      "PPrubberDuck");
#pragma endregion.

  auto blur = ecs.entity("blur").set<ShaderFiles>(
      {"Shaders\\PostProcess\\Basic.vert", "Shaders\\PostProcess\\Blur.frag"});
  auto none_effect =
      ecs.entity("none_effect")
          .set<ShaderFiles>({"Shaders\\PostProcess\\Basic.vert",
                             "Shaders\\PostProcess\\Basic.frag"});
  auto grayscale =
      ecs.entity("grayscale")
          .set<ShaderFiles>({"Shaders\\PostProcess\\Basic.vert",
                             "Shaders\\PostProcess\\Grayscale.frag"});
  auto inversion =
      ecs.entity("inversion")
          .set<ShaderFiles>({"Shaders\\PostProcess\\Basic.vert",
                             "Shaders\\PostProcess\\Inversion.frag"});

  GLuint modelMatrices;
  if (uid_duck != 0) {
    glCreateBuffers(1, &modelMatrices);
    glNamedBufferStorage(modelMatrices, 2 * sizeof(glm::mat4), nullptr,
                         GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices);
    auto name = ModelLoader::GetInstance()->GetName(uid_duck);

    // temp split them up to each single component to test
    ecs.entity(name.c_str())
        .set<Component::Model>(
            {.hProgram = handle,
             .hVAO = ModelLoader::GetInstance()->GetVaoHandle(uid_duck),
             .hVertices = ModelLoader::GetInstance()->GetVboHandle(uid_duck),
             .hMatrices = modelMatrices,
             .uIndicesSize = static_cast<unsigned int>(
                 ModelLoader::GetInstance()->GetIndicesCount(uid_duck))})
        .set<Component::Texture>({.hHandle = texture_handle})
        .set<Component::Position>({{1.0f, 1.0f, 1.0f}})
        .set<PostProcess>({.backupHandles = {blur.get<Program>()->handle,
                                             none_effect.get<Program>()->handle,
                                             grayscale.get<Program>()->handle,
                                             inversion.get<Program>()->handle},
                           .handle = blur.get<Program>()->handle});
  }
}
void LightTest(world &ecs) {
#pragma region Model Loading
  auto uid = TextureLoader::GetInstance().LoadAndCompile(
      R"(D:\VSProject\Graphic\MonkeyRender\bin\data\glTF-Sample-Models\2.0\Box With Spaces\glTF\glTF Logo With Spaces.png)"
      //	        R"(data/wall.jpg)"
  );
  auto texture_handle = TextureLoader::GetInstance().GetHandle(uid);

  auto uid_duck = ModelLoader::GetInstance()->LoadModelAndUpLoad(
      R"(data\glTF-Sample-Models\2.0\Box With Spaces\glTF\Box With Spaces.gltf)",
      //      "data\\glTF-Sample-Models\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf",
      //      R"(data\Container\Container.obj)",
      "LightTestBox");
#pragma endregion.

#pragma region Light
  auto post_handle = ProgramLoader::GetInstance()->LoadAndLink(
      {
          "Shaders\\PostProcess\\Basic.vert",
          "Shaders\\PostProcess\\Blur.frag",
      },
      "screen_shader");
  auto light_handle = ProgramLoader::GetInstance()->LoadAndLink(
      {
          "Shaders\\LightShader\\Light.vert",
          "Shaders\\LightShader\\Light.frag",
      },
      "light_shader");
#pragma endregion

  ecs.entity("PointLight").add<PointLight>();

  GLuint modelMatrices;
  if (uid_duck != 0) {
    glCreateBuffers(1, &modelMatrices);
    glNamedBufferStorage(modelMatrices, 2 * sizeof(glm::mat4), nullptr,
                         GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices);
    auto name = ModelLoader::GetInstance()->GetName(uid_duck);

    // temp split them up to each single component to test
    ecs.entity(name.c_str())
        .set<Component::Model>(
            {.hProgram = light_handle,  // light shader
             .hVAO = ModelLoader::GetInstance()->GetVaoHandle(uid_duck),
             .hVertices = ModelLoader::GetInstance()->GetVboHandle(uid_duck),
             .hMatrices = modelMatrices,
             .uIndicesSize = static_cast<unsigned int>(
                 ModelLoader::GetInstance()->GetIndicesCount(uid_duck))})
        .set<Component::Texture>({.hHandle = texture_handle})
        .set<Component::Position>({{1.0f, 1.0f, 1.0f}});
  }
}
void ShaderObject(world &ecs) {
  auto e = ecs.entity("ShaderTest")
               .set<ShaderFile>({"Shaders\\LightShader\\Light.vert",
                                 "Shaders\\LightShader\\Light.frag"});
}
int main() {
  Core::GetInstance()->Initialize(196, 8);

  Core::GetInstance()->EnableRest();
  Core::GetInstance()
      ->Import<Module::InputModule>()  // input operation
      .Import<Module::CameraModule>()  // camera operation
      .Import<Module::PerFrameDataModule>()
      .Import<ProgramImporter>()
      .Import<ModelImporter>()
      .Import<LightModule>()
      .Import<BaseRenderModule>()
      .Import<ForwardRender>()
      .Import<DefferedRender>()
      .Import<PostProcessModule>()

      .Import<Module::GridModule>()
      .Import<Module::ImGuiRenderModule>()
      .Import<AntiAliasingConfigModule>();

  CreateCamera(Core::GetInstance()->GetWorld());
//  ShaderObject(Core::GetInstance()->GetWorld());
  LightTest(Core::GetInstance()->GetWorld());
  while (Core::GetInstance()->OnUpdate())
    ;

  Core::GetInstance()->Release();
  ShaderLoader::GetInstance()->CleanUpAll();
  ProgramLoader::GetInstance()->CleanUpAll();
  TextureLoader::GetInstance().CleanUpAll();

  return 0;
}
