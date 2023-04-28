#pragma once

#include <glm/glm.hpp>

#include "CommonDef.hpp"

COMP_BGN(RenderComp)
struct PerFrameDataComp {
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec4 cameraPos;
  
  Handle buffer{};
};
SIZE_OF(PerFrameDataComp);

struct BaseRenderer {
  Handle handle{};
  Handle vao{};  // vao should not be here
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

struct PostProcess {
  Handle handle{};
  Handle vao{};
};

struct FrameBuffer {
  Handle handle{};

  Handle colorHandle{};
  Handle depthHandle{};
};

struct ShadowMap {
  Handle shadowProgram{};
};

struct GBuffer {
  Handle handle{};

  Handle positionHandle{};
  Handle NormalHandle{};
  Handle AlbedoHandle{};

  Handle depthHandle{};
};

struct DrawIndirectCommand {
  unsigned int count;
  unsigned int instanceCount;
  unsigned int firstIndex;
  unsigned int baseVertex;
  unsigned int baseInstance;
};
COMP_END(RenderComp)