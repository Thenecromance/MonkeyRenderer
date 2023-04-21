//
// Created by Thenecromance on 2023/4/6.
//
#include "ImGuiRenderModule.hpp"

#include <flecs.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "InputComponents.hpp"
#include "Logger.hpp"
#include "OpenGLApp.hpp"
#include "ShaderLoader.hpp"

using namespace Monkey::Component;

struct InternalImGuiStart {};
MOD_BGN(ImGuiRenderer)

unsigned int ImGuiRenderModule::texture_ = 0;
unsigned int ImGuiRenderModule::vao_ = 0;
unsigned int ImGuiRenderModule::handle_ = 0;
GBuffer *ImGuiRenderModule::vertices_ = nullptr;
GBuffer *ImGuiRenderModule::elements_ = nullptr;
GBuffer *ImGuiRenderModule::perFrame_ = nullptr;

void ImGuiRenderModule::Renderer(ImGuiComp &comp) {
  if (vertices_ == nullptr) return;
  if (elements_ == nullptr) return;
  if (perFrame_ == nullptr) return;

  //  ImGui::ShowDemoWindow();
  ImGui::Render();
  ImDrawData *draw_data = ImGui::GetDrawData();
  if (!draw_data) return;
  glBindTextures(0, 1, &texture_);
  glBindVertexArray(vao_);

  glUseProgram(handle_);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);

  const float L = draw_data->DisplayPos.x;
  const float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
  const float T = draw_data->DisplayPos.y;
  const float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
  const glm::mat4 orthoProjection = glm::ortho(L, R, B, T);
  glNamedBufferSubData(perFrame_->handle(), 0, sizeof(glm::mat4),
                       glm::value_ptr(orthoProjection));

  int width, height;
  /*glfwGetWindowSize((GLFWwindow *)OpenGLApp::GetInstance()->GetWindow(),
     &width, &height);*/
  OpenGLApp::GetInstance()->GetWindowSize(width, height);

  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const ImDrawList *cmd_list = draw_data->CmdLists[n];
    glNamedBufferSubData(
        vertices_->handle(), 0,
        (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
        cmd_list->VtxBuffer.Data);
    glNamedBufferSubData(
        elements_->handle(), 0,
        (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
        cmd_list->IdxBuffer.Data);

    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
      const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
      const ImVec4 cr = pcmd->ClipRect;
      glScissor((int)cr.x, (int)((float)height - cr.w), (int)(cr.z - cr.x),
                (int)(cr.w - cr.y));
      glBindTextureUnit(0, (GLuint)(intptr_t)pcmd->TextureId);
      glDrawElementsBaseVertex(
          GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT,
          (void *)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)),
          (GLint)pcmd->VtxOffset);
    }
  }

  glScissor(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
  glDisable(GL_SCISSOR_TEST);
}

void NewFrame(const InternalImGuiStart &start) {
  int width, height;
  OpenGLApp::GetInstance()->GetWindowSize(width, height);
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)width, (float)height);
  ImGui::NewFrame();
}

void HandleInput(InputController &input, ImGuiComp &comp) {
  auto &io = ImGui::GetIO();

  memcpy(io.MouseDown, input.mouseButton.bPressed, 5);

  io.MousePos = ImVec2((float)input.cursorPos.x, (float)input.cursorPos.y);

  {
    io.MouseWheelH =
        (float)input.scrollButton.xoffset;  // this might be useless, most mouse
                                            // does not support x wheel
    io.MouseWheel = (float)input.scrollButton.yoffset;
  }

  memset(io.KeysDown, 0, IM_ARRAYSIZE(io.KeysDown));

  for (short &i : input.keyBoard.bPressed) {
    if (i == 0) continue;
    io.KeysDown[i] = true;
  }
  io.KeyCtrl =
      io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io.KeyShift =
      io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
}

void ImGuiKeyMapReMapping() {
  auto &io = ImGui::GetIO();

  // from key 0 to key F12 all in the same place , just use this to build mapper
  for (int i = ImGuiKey_0; i <= ImGuiKey_9; ++i) {
    io.KeyMap[i] = GLFW_KEY_0 + i - ImGuiKey_0;
  }
  // the same as above
  for (int i = ImGuiKey_Keypad0; i <= ImGuiKey_Keypad9; ++i) {
    io.KeyMap[i] = GLFW_KEY_KP_0 + i - ImGuiKey_Keypad0;
  }
  for (int i = ImGuiKey_A; i <= ImGuiKey_Z; ++i) {
    io.KeyMap[i] = GLFW_KEY_A + i - ImGuiKey_A;
  }
  // because most of the function keys ImGui and GLFW have totally different key
  // name, so it could be only use this fool ways to generate it

  io.KeyMap[ImGuiKey_None] = GLFW_KEY_UNKNOWN;
  io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
  io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_LeftCtrl] = GLFW_KEY_LEFT_CONTROL;
  io.KeyMap[ImGuiKey_LeftShift] = GLFW_KEY_LEFT_SHIFT;
  io.KeyMap[ImGuiKey_LeftAlt] = GLFW_KEY_LEFT_ALT;
  io.KeyMap[ImGuiKey_LeftSuper] = GLFW_KEY_LEFT_SUPER;
  io.KeyMap[ImGuiKey_RightCtrl] = GLFW_KEY_RIGHT_CONTROL;
  io.KeyMap[ImGuiKey_RightShift] = GLFW_KEY_RIGHT_SHIFT;
  io.KeyMap[ImGuiKey_RightAlt] = GLFW_KEY_RIGHT_ALT;
  io.KeyMap[ImGuiKey_RightSuper] = GLFW_KEY_RIGHT_SUPER;
  io.KeyMap[ImGuiKey_Menu] = GLFW_KEY_MENU;
  io.KeyMap[ImGuiKey_Apostrophe] = GLFW_KEY_APOSTROPHE;     // '
  io.KeyMap[ImGuiKey_Comma] = GLFW_KEY_COMMA;               // ,
  io.KeyMap[ImGuiKey_Minus] = GLFW_KEY_MINUS;               // -
  io.KeyMap[ImGuiKey_Period] = GLFW_KEY_PERIOD;             // .
  io.KeyMap[ImGuiKey_Slash] = GLFW_KEY_SLASH;               // /
  io.KeyMap[ImGuiKey_Semicolon] = GLFW_KEY_SEMICOLON;       // ;
  io.KeyMap[ImGuiKey_Equal] = GLFW_KEY_EQUAL;               // =
  io.KeyMap[ImGuiKey_LeftBracket] = GLFW_KEY_LEFT_BRACKET;  // [
  io.KeyMap[ImGuiKey_Backslash] =
      GLFW_KEY_BACKSLASH;  // \ (this text inhibit multiline comment caused by
                           // backslash)
  io.KeyMap[ImGuiKey_RightBracket] = GLFW_KEY_RIGHT_BRACKET;  // ]
  io.KeyMap[ImGuiKey_GraveAccent] = GLFW_KEY_GRAVE_ACCENT;    // `
  io.KeyMap[ImGuiKey_CapsLock] = GLFW_KEY_CAPS_LOCK;
  io.KeyMap[ImGuiKey_ScrollLock] = GLFW_KEY_SCROLL_LOCK;
  io.KeyMap[ImGuiKey_NumLock] = GLFW_KEY_NUM_LOCK;
  io.KeyMap[ImGuiKey_PrintScreen] = GLFW_KEY_PRINT_SCREEN;
  io.KeyMap[ImGuiKey_Pause] = GLFW_KEY_PAUSE;
  io.KeyMap[ImGuiKey_KeypadDecimal] = GLFW_KEY_KP_DECIMAL;
  io.KeyMap[ImGuiKey_KeypadDivide] = GLFW_KEY_KP_DIVIDE;
  io.KeyMap[ImGuiKey_KeypadMultiply] = GLFW_KEY_KP_MULTIPLY;
  io.KeyMap[ImGuiKey_KeypadSubtract] = GLFW_KEY_KP_SUBTRACT;
  io.KeyMap[ImGuiKey_KeypadAdd] = GLFW_KEY_KP_ADD;
  io.KeyMap[ImGuiKey_KeypadEnter] = GLFW_KEY_KP_ENTER;
  io.KeyMap[ImGuiKey_KeypadEqual] = GLFW_KEY_KP_EQUAL;
}

ImGuiRenderModule::ImGuiRenderModule(world &ecs) {
  ecs.module<ImGuiRenderModule>();
  Logger::get("Module")->info("System::ImGuiRenderModule");

  ecs.component<ImGuiComp>("ImGuiComponent")
      .member<float>("width")
      .member<float>("height");

  LoadSystem(ecs);
  LoadEntity(ecs);

  Initialize();
  ImGuiKeyMapReMapping();
}

ImGuiRenderModule::~ImGuiRenderModule() {
  glDeleteVertexArrays(1, &vao_);
  delete vertices_;
  delete elements_;
  delete perFrame_;

  Logger::get<ImGuiRenderModule>()->info("released ImGuiRenderModule");
}

void ImGuiRenderModule::LoadSystem(world &ecs) {
  ecs.system<InputController, ImGuiComp>("System::ImGuiInputControl")
      .kind(flecs::PostLoad)
      .each(HandleInput);

  ecs.system<const InternalImGuiStart>("System::ImGuiNewFrame")
      .kind(flecs::PreFrame)
      .each(NewFrame);

  ecs.system<ImGuiComp>("System::ImGuiRenderer")
      .kind(flecs::OnStore)
      .each(Renderer);
}

void ImGuiRenderModule::LoadEntity(world &ecs) {
  ecs.entity("Entity::ImGuiStartFrameDriver").add<InternalImGuiStart>();
  ecs.entity("Entity::ImGuiShowDemo")
      .set<ImGuiComp>({800.0f, 600.0f})
      .add<InputController>();
}

void ImGuiRenderModule::LoadImGuiFont(const char *font_path) {
  ImGuiIO &io = ImGui::GetIO();
  ImFontConfig cfg = ImFontConfig();
  cfg.FontDataOwnedByAtlas = false;
  cfg.RasterizerMultiply = 1.5f;
  cfg.SizePixels = 15.0f;
  cfg.PixelSnapH = true;
  cfg.OversampleH = 4;
  cfg.OversampleV = 4;
  ImFont *Font = io.Fonts->AddFontFromFileTTF(font_path, cfg.SizePixels, &cfg);

  unsigned char *pixels = nullptr;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  glCreateTextures(GL_TEXTURE_2D, 1, &texture_);
  glTextureParameteri(texture_, GL_TEXTURE_MAX_LEVEL, 0);
  glTextureParameteri(texture_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTextureParameteri(texture_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureStorage2D(texture_, 1, GL_RGBA8, width, height);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTextureSubImage2D(texture_, 0, 0, 0, width, height, GL_RGBA,
                      GL_UNSIGNED_BYTE, pixels);

  io.Fonts->TexID = (ImTextureID)(intptr_t)texture_;
  io.FontDefault = Font;
  io.DisplayFramebufferScale = ImVec2(1, 1);
}

void ImGuiRenderModule::defaultInitImGui() {
  ImGui::CreateContext();

  ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  /*  ImGui::GetIO().ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls*/

  LoadImGuiFont("data/font/Monaco.ttf");
}

void ImGuiRenderModule::InitializeParams() {
  vertices_ = new GBuffer(128 * 1024, nullptr, GL_DYNAMIC_STORAGE_BIT);
  elements_ = new GBuffer(256 * 1024, nullptr, GL_DYNAMIC_STORAGE_BIT);
  perFrame_ = new GBuffer(sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);

  Logger::get("ImGuiRenderModule")
      ->debug("vertices handle:{}, elements handle:{}, perFrame handle:{}",
              vertices_->handle(), elements_->handle(), perFrame_->handle());
}

void ImGuiRenderModule::Initialize() {
  InitializeParams();

  glCreateVertexArrays(1, &vao_);

  glVertexArrayElementBuffer(vao_, elements_->handle());
  glVertexArrayVertexBuffer(vao_, 0, vertices_->handle(), 0,
                            sizeof(ImDrawVert));

  glEnableVertexArrayAttrib(vao_, 0);
  glEnableVertexArrayAttrib(vao_, 1);
  glEnableVertexArrayAttrib(vao_, 2);

  glVertexArrayAttribFormat(vao_, 0, 2, GL_FLOAT, GL_FALSE,
                            IM_OFFSETOF(ImDrawVert, pos));
  glVertexArrayAttribFormat(vao_, 1, 2, GL_FLOAT, GL_FALSE,
                            IM_OFFSETOF(ImDrawVert, uv));
  glVertexArrayAttribFormat(vao_, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                            IM_OFFSETOF(ImDrawVert, col));

  glVertexArrayAttribBinding(vao_, 0, 0);
  glVertexArrayAttribBinding(vao_, 1, 0);
  glVertexArrayAttribBinding(vao_, 2, 0);

  glBindBufferBase(GL_UNIFORM_BUFFER, 7, perFrame_->handle());

  Logger::get("ImGuiRenderModule")->debug("vao_ handle:{}", vao_);

  {
    handle_ = ProgramLoader::GetInstance()->LoadAndLink(
        {"Shaders/ImGuiShader/Imgui.vs", "Shaders/ImGuiShader/Imgui.fs"},
        "ImGuiRenderer");

    Logger::get("ImGuiRenderModule")->debug("Program handle:{}", handle_);
  }
  defaultInitImGui();
}

MOD_END(ImGuiRenderer)