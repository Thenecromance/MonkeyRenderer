
#pragma once

#include "Patterns/SingleTon.hpp"
#include "Resources/ResourceLoader.hpp"

class TextureLoader : public ResourceLoader,
                      public EagerSingleTon<TextureLoader> {
 public:
  TextureLoader();

  Buffer InsertBufferInfo(const std::string &path,
                          std::vector<char> &buffer) override;
};
