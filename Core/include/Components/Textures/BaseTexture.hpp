#pragma once

#include <CommonDef.hpp>
#include <Marcos/Marcos.hpp>

#include "TextureEnum.hpp"

COMP_BGN(BaseTexture)

struct Texture {
  Handle hHandle{};  // Handle to the texture
  TextureType Type = TextureType::Normal;
};

COMP_END(BaseTexture)