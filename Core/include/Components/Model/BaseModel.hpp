#pragma once
#include <CommonDef.hpp>
#include <Marcos/Marcos.hpp>

COMP_BGN(Model)

struct Model {
  Handle hProgram;
  Handle hVAO;
  Handle hVertices;
  Handle hMatrices;
  unsigned int uIndicesSize;
};

COMP_END(Model)