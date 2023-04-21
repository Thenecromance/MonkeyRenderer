

#include "Buffer/Buffer.hpp"

#include <glad/glad.h>

#include "utils/Logger.hpp"

GBuffer::GBuffer(unsigned int size, void *pData, unsigned int uiFlag)
    : size_(size), uiFlag_(uiFlag), bAllocated_(true) {
  glCreateBuffers(1, &uiHandle_);
  glNamedBufferStorage(uiHandle_, size_, pData, uiFlag_);
}

GBuffer::~GBuffer() {
  if (uiHandle_ != 0) glDeleteBuffers(1, &uiHandle_);
}

GBuffer::GBuffer() : size_(0), uiFlag_(0), bAllocated_(false) {
  glCreateBuffers(1, &uiHandle_);
}

unsigned int GBuffer::Update(unsigned int size, void *pData,
                             unsigned int uiFlag) {
  if (bAllocated_) {
    Logger::get("GBuffer")->error("{} is already allocated", uiHandle_);
    return uiHandle_;
  }

  size_ = size;
  uiFlag_ = uiFlag;
  bAllocated_ = true;
  glNamedBufferStorage(uiHandle_, size_, pData, uiFlag_);
  return uiHandle_;
}
unsigned int GBuffer::ForceUpdate(unsigned int size, void *pData,
                                  unsigned int uiFlag) {
  if (bAllocated_) {
    Logger::get<GBuffer>()->warn(
        "Buffer {} is already allocated, you are using force update",
        uiHandle_);
  }
  bAllocated_ = false;
  return Update(size, pData, uiFlag);
}
