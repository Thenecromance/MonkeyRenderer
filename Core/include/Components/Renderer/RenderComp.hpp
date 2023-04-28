#pragma once

#include <glm/glm.hpp>
#include <string>

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

  Handle vao{};
  unsigned int drawType{0x0004};
};
SIZE_OF(BaseRenderer);

struct ForwardRenderer {
  Handle handle{};
  unsigned int drawType{0x0004};
};
SIZE_OF(ForwardRenderer);

/// @brief deffered renderer if object is marked by this ,
struct DefferedRenderer {
  Handle geomHandle{};
  Handle lightHandle{};
  unsigned int drawType{0x0004};
};
SIZE_OF(DefferedRenderer);

/// @brief post process effect
struct PostProcess {
  Handle handle{};
  Handle vao{};
};
SIZE_OF(PostProcess);

/// @brief frame buffer for advanced rendering operation
struct FrameBuffer {
  Handle handle{};

  Handle colorHandle{};
  Handle depthHandle{};
};
SIZE_OF(FrameBuffer);

/// @brief geometry buffer for deffered rendering
struct GBuffer {
  Handle handle{};

  Handle positionHandle{};
  Handle NormalHandle{};
  Handle AlbedoHandle{};

  Handle depthHandle{};
};
SIZE_OF(GBuffer);

struct DrawIndirectCommand {
  unsigned int count;
  unsigned int instanceCount;
  unsigned int firstIndex;
  unsigned int baseVertex;
  unsigned int baseInstance;
};
SIZE_OF(DrawIndirectCommand);

struct ShadowMapComp {
  std::string file_path{};
  Handle handle{};
};
SIZE_OF(ShadowMapComp);


COMP_END(RenderComp)