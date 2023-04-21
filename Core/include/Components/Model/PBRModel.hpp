#pragma once
#include <CommonDef.hpp>
#include <Marcos/Marcos.hpp>
#include <array>

#include "BaseModel.hpp"
COMP_BGN(PBRModel)

/// @brief so far PBR model means using PBR shader and PBR textures ,so the
/// different between PBRModel and BaseModel is that PBRModel has multi textures
/// and BaseModel has only one texture
/// maybe I can remove the PBR data info from PBRModel and put it into other
/// components like PBRTexture
struct PBRModel {};

COMP_END(PBRModel)