#pragma once

#include <CommonDef.hpp>
#include <Marcos/Marcos.hpp>
#include <array>

#include "TextureEnum.hpp"

COMP_BGN(PBRTexture)

/// @brief PBR texture component is mean to try to use PBR rendering the model,
/// and the different between PBRTexture and BaseTexture is that PBRTexture has
/// multi textures and BaseTexture has only one texture so just use an array to
/// store the textures and use the TextureType to index the array then we can
/// get the texture we want, also the textures will be the clean and clear in
/// the local cache when the PBRTexture component is destroyed about the data's
/// file cache will let the textureLoader to handle and manage it in other ways
/// ,if PBR texture missing some texture , it could be also change back to
/// normal texture rendering
struct PBRTexture {
  std::array<Handle, TextureType::TextureTypeCount> hTextures;
};

using PBRMaterial = PBRTexture;

COMP_END(PBRTexture)