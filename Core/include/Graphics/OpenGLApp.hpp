#pragma once
#pragma once

#include "Patterns/SingleTon.hpp"

struct GLConfig {
  // openGLVersion Setting
  int major{4};
  int minor{6};
  int profile{0x00032001};  // GLFW_CORE_PROFILE:0x00032001

  // default background color
  float r{0.5f}, g{0.5f}, b{0.5f}, a{1.0f};  // default color

  // Title Name
  const char *name = "MonkeyRender";
};

/// @brief GLFW Wrapper
class OpenGLApp : public LazySingleTon<OpenGLApp> {
  struct WindowSize {
    int width;
    int height;
  };

 protected:
 public:
  ~OpenGLApp() override{};

  bool Initialize();

  void SetIcon(int width, int height, unsigned char *img);

  void SetAppConfig(GLConfig &cfg);

  GLConfig &GetConfig() { return _cfg; }

  GLConfig CopyConfig() { return _cfg; }

  void *GetWindow() { return pWindow; }

  void GetWindowSize(int &width, int &height);

 public:
  bool OnUpdateBegin();

  void OnUpdateEnd();

 private:
  void _MergeConfig(GLConfig &cfg);

 private:
  void _SetOpenGLVersion() const;

  bool _CreateWindow();

  void _SetWindowTitle(const char *name);

  static bool _LoadGlad();

  WindowSize _GetWindowSize();

 private:
  void *pWindow{};
  GLConfig _cfg;
};

class SystemInfo {
 public:
  static const char *GetGLVersion();

  static const char *GetGLSLVersion();

  static const char *GetGLVendor();

  static const char *GetGLRenderer();

  static int GetGLExtensionsCount();

  static const char *GetGLExtensions(const int &index);

  static const char *GetGLFWVersion();
};
