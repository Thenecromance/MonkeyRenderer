#pragma once
#include <vector>

#include "CommonDef.hpp"

// Shader Storage Buffer Object
struct SSBO {};

/// @brief this object is working for creating gpu buffer, some times when
/// trying to load a large amount of data, uniform buffer is not enough, so we
/// need to use SSBO(over 4.0 version) to load data. or use texture buffer.
class BufferCreator {
 public:
  BufferCreator();
  ~BufferCreator();

  Handle CreateBuffer();

 private:
  void GetMaxBufferSupported();

  /// @brief directly create a buffer, this buffer is not bind to any shader
  /// @return the buffer handle
  Handle _CreateBuffer();
  /// @brief create a Shader Storage Buffer Object, allows us to allocate a
  /// large buffer size
  /// @return buffer handle
  Handle _CreateSSBO();
  /// @brief create a texture buffer as uniform buffer and load more data on it
  /// , but need to processing in shader
  /// @return buffer  handle
  Handle _CreateTextureAsBuffer();

 private:
  static int max_buffer_supported_;
};