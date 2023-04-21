#pragma once

#include "CommonDef.hpp"
#include "Marcos/Marcos.hpp"

COMP_BGN(Render)
struct RenderComponents {
  Handle ProgramHandle;
  int iIndicesCount{3};
};

COMP_END(Render)