#pragma once

#include <glm/glm.hpp>

#include "CommonDef.hpp"

/*enum PBRTextureLocation {
  Albedo,
  Normal,
  Metallic,
  Roughness,
  AO,
  Emissive,
  Count
};*/

struct TextureBuffer {
  Handle hHandle{};
  // TextureType Type = TextureType::Normal;
};

struct PerFrameData {
  glm::mat4 view;
  glm::mat4 proj;
  glm::vec4 cameraPos;

  unsigned int handle{};
};

struct BaseRenderer {
  Handle handle{};
  Handle vao{};
  unsigned int drawType{0x0004};
};

struct ForwardRenderer {
  Handle handle{};
};

struct DefferedRenderer {
  Handle geomHandle{};
  Handle lightHandle{};
  unsigned int drawType{0x0004};
};

#include <vector>
struct PostProcess {
  std::vector<Handle> backupHandles{};
  Handle handle{};
  Handle vao{};
};

struct FrameBuffer {
  Handle handle{};
  TextureHandle colorHandle{};
  TextureHandle depthHandle{};
};

struct gBuffer {
  Handle handle{};

  TextureHandle positionHandle{};
  TextureHandle NormalHandle{};
  TextureHandle AlbedoHandle{};
};

struct DrawIndirectCommand {
  unsigned int count;
  unsigned int instanceCount;
  unsigned int firstIndex;
  unsigned int baseVertex;
  unsigned int baseInstance;
};
