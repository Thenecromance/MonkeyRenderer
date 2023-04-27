#pragma once

#include <string>

#include "CommonDef.hpp"

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