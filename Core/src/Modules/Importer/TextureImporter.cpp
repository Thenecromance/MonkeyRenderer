#include "TextureImporter.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <stb_image.h>

#include "File.hpp"
#include "Logger.hpp"
#include "Texture.hpp"

/// @brief Load texture from file and upload to GPU instantly
/// @param path the file path
/// @param type GL_TEXTURE_2D as default
/// @param handle texture handle
void LoadByAssimp(std::string& path, unsigned int type, Handle& handle) {
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    glGenTextures(1, &handle);
    glBindTexture(type, handle);
    glTexImage2D(type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(type);
  } else {
    Logger::get<TextureImporter>()->info("Failed to load texture:{}", path);
  }
  stbi_image_free(data);
}

void TextureInit(flecs::iter& it, size_t i, Texture& tex) {
  if (tex.loadType == 0) {
    tex.loadType = GL_TEXTURE_2D;
  }
  if (tex.path.empty()) {
    tex.path = "Resources/Textures/Default.png";
  }
}
void LoadTexture(flecs::iter& it, size_t i, Texture& tex) {
  auto self = it.entity(i);
  Handle handle{};
  LoadByAssimp(tex.path, tex.loadType, handle);

  if (handle == 0) {
    Logger::get<TextureImporter>()->info("Failed to load texture:{}", tex.path);
  } else {
    std::string name = tex.name;
    if (name.empty()) {
      name = File(tex.path).getFileNameWithoutExtension();
    }

    self.disable<Texture>().set<TextureHandle>(
        {.handle = handle, .type = tex.loadType, .name = name});
  }
}
void ReleaseTexture(flecs::iter& it, size_t i, TextureHandle& tex) {
  glDeleteTextures(1, &tex.handle);
}

TextureImporter::TextureImporter(world& ecs) {
  LoadObserver(ecs);
  RegisterComponent(ecs);
}

void TextureImporter::LoadObserver(world& ecs) {
  ecs.observer<Texture>().event(flecs::OnAdd).each(TextureInit);
  ecs.observer<Texture>().event(flecs::OnSet).each(LoadTexture);
  ecs.observer<TextureHandle>().event(flecs::OnRemove).each(ReleaseTexture);
}

void TextureImporter::RegisterComponent(world& ecs) {
  ecs.component<Texture>()
      .member<std::string>("path")
      .member<unsigned int>("type")
      .member<std::string>("name");

  ecs.component<Texture>()
      .member<Handle>("handle")
      .member<unsigned int>("type")
      .member<std::string>("name");
}
