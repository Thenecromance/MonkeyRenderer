#pragma once

#include <string>
#include "Marcos/Marcos.hpp"
#include "CommonDef.hpp"

COMP_BGN(Texture)
/// @brief Texture file
struct Texture {
  std::string path{};
  unsigned int loadType{};
  std::string name{};
};

/// @brief Texture has been upload to GPU
struct TextureHandle {
  Handle handle{};
  unsigned int type{};
  std::string name{};
};

// PBR Warpped
struct Material {
  Handle baseColor{};

  Handle metallic_Roughness{};

  Handle normal{};

  Handle occulusion{};

  Handle emissive{};
};


COMP_END(Texture)