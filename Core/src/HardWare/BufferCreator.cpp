#include "BufferCreator.hpp"

#include <glad/glad.h>

#include "Logger.hpp"

// on my pc, this value should be 65535 bytes
int BufferCreator::max_buffer_supported_ = 0;

BufferCreator::BufferCreator() {}

Handle BufferCreator::CreateBuffer() {
  Handle handle;

  glCreateBuffers(1, &handle);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
  return handle;
}

void BufferCreator::GetMaxBufferSupported() {
  glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_buffer_supported_);
  Logger::get<BufferCreator>()->debug("max buffer supported: {} bytes",
                                      max_buffer_supported_);
}
