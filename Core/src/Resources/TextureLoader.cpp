//
// Created by Thenecromance on 2023/3/30.
//
#define STB_IMAGE_IMPLEMENTATION

#include "TextureLoader.hpp"

#include "Buffer/Buffer.hpp"
#include "Logger.hpp"
#include "TextureWorker.hpp"
#include "stb_image.h"

TextureLoader::TextureLoader() {
  mName = "TextureLoader";
  pWorker = new Monkey::TextureWorker();
}

Buffer TextureLoader::InsertBufferInfo(const std::string &path,
                                       std::vector<char> &buffer) {
  int width, height, nrChannels;
  auto pImg =
      stbi_load_from_memory((unsigned char *)buffer.data(), buffer.size(),
                            &width, &height, &nrChannels, 3);
  //	auto pImg = stbi_load(path.c_str(), &width, &height, &nrChannels, 3);
  if (pImg == nullptr) {
    Logger::get<TextureLoader>()->error("{}", stbi_failure_reason());
    Logger::get("TextureLoader")->info("Failed to load texture: {}", path);
    assert(false);
  }
  //	Logger::get("TextureLoader")->info("Texture: {} loaded", pImg);
  const int imgSize = width * height * nrChannels;

  Buffer buffer_((const char *)pImg, imgSize, eCustomStart);
  buffer_.set_value(TextureParamIndex::eMadeBindless, 0);
  buffer_.set_value(TextureParamIndex::eWidth, width);
  buffer_.set_value(TextureParamIndex::eHeight, height);
  buffer_.set_value(TextureParamIndex::eNrChannels, nrChannels);

  stbi_image_free(pImg);
  return buffer_;
}