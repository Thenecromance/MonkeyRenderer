#pragma once

#include <map>

#include "Buffer/Buffer.hpp"
#include "CommonDef.hpp"
#include "Job.hpp"
#include "Marcos/Marcos.hpp"
#include "Patterns/SingleTon.hpp"

COMP_BGN(ImGuiRenderer)

struct ImGuiBaseComp {
  Handle texture_;
  Handle vao_;
  Handle vertices_;
  Handle elements_;
  Handle perFrame_;
};
COMP_END(ImGuiRenderer)

MOD_BGN(ImGuiRenderer)

class ImGuiRenderer {
 public:
  explicit ImGuiRenderer(world &ecs);

  ~ImGuiRenderer(){};

  static void LoadImGuiFont(const char *font_path);

 private:
  static void LoadSystem(world &ecs);

  void LoadEntity(world &ecs);

  static void Initialize();

  static void defaultInitImGui();

  static void InitializeParams();

 private:
  static void Renderer(Component::ImGuiBaseComp &comp);
};
MOD_END(ImGuiRenderer)
