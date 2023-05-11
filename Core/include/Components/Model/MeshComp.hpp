#pragma once

#include <string>
#include <vector>

#include "CommonDef.hpp"
#include "Marcos/Marcos.hpp"
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

  void DrawElement(unsigned int mode = 0x0004) const;
  void DrawArrays(unsigned int mode, int first, int count) const;
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