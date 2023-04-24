#include <flecs.h>
#include <glad/glad.h>

#include "TextureModule.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "File.hpp"
#include "Logger.hpp"
#include "Texture.hpp"
MOD_BGN(TextureModule)
/// @brief Load texture from file and upload to GPU instantly
/// @param path the file path
/// @param type GL_TEXTURE_2D as default
/// @param handle texture handle
void LoadByStb(std::string& path, unsigned int type, Handle& handle) {
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    /* glCreateTextures(1, &handle);
     glBindTexture(type, handle);
     glTexImage2D(type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                  data);
     glGenerateMipmap(type);*/

    glCreateTextures(type, 1, &handle);
    glTextureParameteri(handle, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(handle, 1, GL_RGB8, width, height);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGB,
                        GL_UNSIGNED_BYTE, data);
  } else {
    Logger::get<TextureModule>()->error("Failed to load texture:{}", path);
  }
  stbi_image_free(data);
}

void LoadTexture(flecs::entity self, Texture& tex) {
  Handle handle{};
  if (tex.loadType == 0) {
    tex.loadType = GL_TEXTURE_2D;
  }
  if (tex.path.empty()) {
    tex.path = "Resources/Textures/Default.png";
  }
  LoadByStb(tex.path, tex.loadType, handle);

  if (handle == 0) {
    Logger::get<TextureModule>()->info("Failed to load texture:{}", tex.path);
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

TextureModule::TextureModule(world& ecs) {
  ecs.module<TextureModule>();
  LoadObserver(ecs);
  RegisterComponent(ecs);
}

void TextureModule::LoadObserver(world& ecs) {
  ecs.observer<Texture>().event(flecs::OnSet).each(LoadTexture);
  ecs.observer<TextureHandle>().event(flecs::OnRemove).each(ReleaseTexture);
}

void TextureModule::RegisterComponent(world& ecs) {

  ecs.component<TextureHandle>().member<Handle>("handle").member<unsigned int>(
      "type")      ;
}
MOD_END(TextureModule)