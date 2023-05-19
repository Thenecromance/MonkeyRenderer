

#include "OpenGLApp.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Logger.hpp"
#include <iostream>
#define CAST(ptr) static_cast<GLFWwindow *>(ptr)

//TODO: lol
// this object don't need to continue to develop, in Experimental\src\Module\GLFWwindowModule.cpp has a better ways to merge it into the engine.


void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                      GLsizei length, GLchar const *message,
                      void const *user_param) {
  auto const src_str = [source]() {
    switch (source) {
      case GL_DEBUG_SOURCE_API:
        return "API";
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "WINDOW SYSTEM";
      case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "SHADER COMPILER";
      case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "THIRD PARTY";
      case GL_DEBUG_SOURCE_APPLICATION:
        return "APPLICATION";
      case GL_DEBUG_SOURCE_OTHER:
        return "OTHER";
    }
    return "";
  }();

  auto const type_str = [type]() {
    switch (type) {
      case GL_DEBUG_TYPE_ERROR:
        return "ERROR";
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DEPRECATED_BEHAVIOR";
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UNDEFINED_BEHAVIOR";
      case GL_DEBUG_TYPE_PORTABILITY:
        return "PORTABILITY";
      case GL_DEBUG_TYPE_PERFORMANCE:
        return "PERFORMANCE";
      case GL_DEBUG_TYPE_MARKER:
        return "MARKER";
      case GL_DEBUG_TYPE_OTHER:
        return "OTHER";
    }
    return "";
  }();

  auto const severity_str = [severity]() {
    switch (severity) {
      case GL_DEBUG_SEVERITY_NOTIFICATION:
        return "NOTIFICATION";
      case GL_DEBUG_SEVERITY_LOW:
        return "LOW";
      case GL_DEBUG_SEVERITY_MEDIUM:
        return "MEDIUM";
      case GL_DEBUG_SEVERITY_HIGH:
        return "HIGH";
    }
    return "";
  }();

  Logger::get("OpenGL Debug")
      ->error("{}: {} {} {}: {}", src_str, type_str, severity_str, id, message);
  //	std::cout << src_str << ", " << type_str << ", " << severity_str << ", "
  //<< id << ": " << message << '\n' << '\n';
}

void initDebug() {
  glDebugMessageCallback(message_callback, nullptr);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                        GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
};

bool OpenGLApp::Initialize() {
  if (!glfwInit()) {
    Logger::get<OpenGLApp>()->error("Failed to initialize GLFW");
    return false;
  }

  _SetOpenGLVersion();
  if (!_CreateWindow()) {
    Logger::get<OpenGLApp>()->error("Failed to create window");
    return false;
  }

  if (!_LoadGlad()) {
    Logger::get<OpenGLApp>()->error("Failed to load glad");
    return false;
  }
  initDebug();

  //  int num = 0;
  //  glGetIntegerv(GL_NUM_EXTENSIONS, &num);
  //  for (int i = 0; i < num; i++) {
  //    std::cout << glGetStringi(GL_EXTENSIONS, i) << std::endl;
  //  }
  return true;
}

void OpenGLApp::SetIcon(int width, int height, unsigned char *img) {
  if (width == 0 || height == 0) {
    return;
  }
  auto *Icons = new GLFWimage{.width = width, .height = height, .pixels = img};
  glfwSetWindowIcon(CAST(pWindow), 1, Icons);
}

void OpenGLApp::SetAppConfig(GLConfig &cfg) { _MergeConfig(cfg); }

bool OpenGLApp::OnUpdateBegin() {
  if (glfwWindowShouldClose(CAST(pWindow))) {
    return false;
  }
  auto [width, height] = _GetWindowSize();
  glViewport(0, 0, width, height);
  glClearColor(_cfg.r, _cfg.g, _cfg.b, _cfg.a);

  // TODO : may be Need to replace ?
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  return true;
}

void OpenGLApp::OnUpdateEnd() {
  glfwSwapBuffers(
      CAST(pWindow));  // the same thing, so it's safe to use static_cast
  glfwPollEvents();
}

void OpenGLApp::_MergeConfig(GLConfig &cfg) {
// I'm a lazy monkey, so just use marcos
#define CMP(val) _cfg.val != cfg.val
#define Apply(val) _cfg.val = cfg.val
  // OpenGL version apply and check
  {
    if (CMP(major) || CMP(minor) || CMP(profile)) {
      Logger::get<OpenGLApp>()->info("OpenGL version changed from {} to {}.{}",
                                     _cfg.major, _cfg.minor, _cfg.profile);
      Apply(major);
      Apply(minor);
      Apply(profile);
      _SetOpenGLVersion();
    }
  }

  // set the default color
  {
    if (CMP(r) || CMP(g) || CMP(b) || CMP(a)) {
      Logger::get<OpenGLApp>()->info(
          "Default color changed from [{} {} {} {}] to [{} {} {} {}]", _cfg.r,
          _cfg.g, _cfg.b, _cfg.a, cfg.r, cfg.g, cfg.b, cfg.a);

      Apply(r);
      Apply(g);
      Apply(b);
      Apply(a);
      // this is use for glClearColor, so don't need to call it
    }
  }

  // title compare
  {
    // compare 2 const char*
    if (CMP(name)) {
      Logger::get<OpenGLApp>()->info("Window title changed from {} to {}",
                                     _cfg.name, cfg.name);

      Apply(name);
      _SetWindowTitle(_cfg.name);
    }
  }
#undef Apply
#undef CMP
}

void OpenGLApp::_SetOpenGLVersion() const {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _cfg.major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _cfg.minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, _cfg.profile);
}

bool OpenGLApp::_CreateWindow() {
  auto info = glfwGetVideoMode(glfwGetPrimaryMonitor());

  // create window and hold window ptr
  pWindow = (void *)glfwCreateWindow(info->width, info->height, _cfg.name,
                                     nullptr, nullptr);

  if (!pWindow) {
    glfwTerminate();
    return false;
  }
  glfwWindowHint(GL_MULTISAMPLE,
                 4);  // Anti Aliasing so far just use glfw default setting
  glfwMakeContextCurrent(CAST(pWindow));
  glfwSwapInterval(1);
  return true;
}

void OpenGLApp::_SetWindowTitle(const char *name) {
  glfwSetWindowTitle(CAST(pWindow), name);
}

bool OpenGLApp::_LoadGlad() {
  if (!gladLoadGL()) {
    return false;
  }
  return true;
}

OpenGLApp::WindowSize OpenGLApp::_GetWindowSize() {
  int width, height;
  glfwGetFramebufferSize(CAST(pWindow), &width, &height);

  return {width, height};
}
void OpenGLApp::GetWindowSize(int &width, int &height) {
  auto [w, h] = _GetWindowSize();
  width = w;
  height = h;
}

#undef CAST
