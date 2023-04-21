#pragma once

#include <map>

#include "Buffer/Buffer.hpp"
#include "Job.hpp"
#include "Marcos/Marcos.hpp"
#include "Patterns/SingleTon.hpp"

COMP_BGN(ImGuiRenderer)
struct ImGuiComp {
  float width;
  float height;
};
COMP_END(ImGuiRenderer)

MOD_BGN(ImGuiRenderer)

class ImGuiRenderModule {
 public:
  explicit ImGuiRenderModule(world &ecs);

  ~ImGuiRenderModule();

  static void LoadImGuiFont(const char *font_path);

 private:
  static void LoadSystem(world &ecs);

  void LoadEntity(world &ecs);

  static void Initialize();

  static void defaultInitImGui();

  static void InitializeParams();

 private:
  static void Renderer(Component::ImGuiComp &comp);

 private:
  static unsigned int texture_;
  static unsigned int vao_;
  static unsigned int handle_;
  static GBuffer *vertices_;
  static GBuffer *elements_;
  static GBuffer *perFrame_;
};

MOD_END(ImGuiRenderer)
