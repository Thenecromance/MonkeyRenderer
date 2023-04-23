#pragma once

#include <string>
#include <vector>
#include "Marcos/Marcos.hpp"
#include "CommonDef.hpp"
COMP_BGN(Mesh)
/// @brief using this component to load the mesh object from the disk
struct MeshFile {
  std::string path;
};

/// @brief gpu operation with mesh handles
struct Mesh {
  Handle vao{};
  Handle Indices{};
  Handle Vertices{};
  Handle Indirect{};
  unsigned int numIndices{};
};
/// @brief mesh data in cache
struct MeshData {
  std::vector<uint32_t> Indices;
  std::vector<float> Vertex;
};

struct DrawData {
  unsigned int meshIndex;
  unsigned int materialIndex;
  unsigned int LOG;
  unsigned int indexOffset;
  unsigned int vertexOffset;
  unsigned int transformIndex;
};

COMP_END(Mesh)