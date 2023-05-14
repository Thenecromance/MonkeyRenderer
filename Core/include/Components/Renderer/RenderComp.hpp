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

struct BaseRenderComp {
  Handle handle{};

  Handle vao{};
  unsigned int drawType{0x0004};
};
SIZE_OF(BaseRenderComp);

struct ForwardRenderComp {
  unsigned int drawType{0x0004};
};
SIZE_OF(ForwardRenderComp);

/// @brief deffered renderer if object is marked by this ,
struct DefferedRenderComp {
  Handle geomHandle{};
  Handle lightHandle{};
};
SIZE_OF(DefferedRenderComp);

/// @brief post process effect
struct PostProcess {
  Handle handle{};
  Handle vao{};
};
SIZE_OF(PostProcess);

struct DrawIndirectCommand {
  unsigned int count;
  unsigned int instanceCount;
  unsigned int firstIndex;
  unsigned int baseVertex;
  unsigned int baseInstance;
};
SIZE_OF(DrawIndirectCommand);

#pragma region FrameBuffer Sections

/// @brief frame buffer for advanced rendering operation
struct FrameBuffer {
  Handle handle{};

  Handle colorHandle{};
  Handle depthHandle{};

  int height{};
  int width{};

 public:
  void Bind();
  void Unbind();
};
SIZE_OF(FrameBuffer);

/// @brief geometry buffer for deffered rendering
struct GBuffer {
  Handle handle{};

  Handle positionHandle{};
  Handle normalHandle{};
  Handle albedoHandle{};

  Handle depthHandle{};
  
  
  int     width{};
  int     height{};
  void Bind();
  void Unbind();
};
SIZE_OF(GBuffer);

struct ShadowMap {
  Handle handle{};

  Handle colorHandle{};
  Handle depthHandle{};

  int height{1024};
  int width{1024};

 public:
  void Bind();
  void Unbind();
};
SIZE_OF(ShadowMap);

struct SSAO {
  Handle handle{};

  Handle colorHandle{};
  Handle depthHandle{};

  int height{};
  int width{};

 public:
  void Bind();
  void Unbind();
};
SIZE_OF(SSAO);

#pragma endregion

COMP_END(RenderComp)