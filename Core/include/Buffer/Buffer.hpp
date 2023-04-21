#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4267)
// temp put here to handle the memory allocation for Buffer
// TODO: allocator for buffer
namespace Allocator {

typedef void (*free_t)(void *);

typedef void *(*malloc_t)(size_t);

static free_t my_free = free;
static malloc_t my_malloc = malloc;
};  // namespace Allocator

/// @brief custom buffer data to handle the data in memory
/// @tparam Type if need send custom params to the buffer this is the class
// template<typename Type = int>
class Buffer {
  typedef int ParamType;
  static constexpr size_t p_size = sizeof(ParamType);

 public:
  /// @brief default ctor
  /// @param buffer buffer data which it will be copied to the buffer
  /// @param count params count
  explicit Buffer(std::vector<char> &buffer, int count = 0) {
    iParams = count;
    _init(buffer.size(), count);
    _allocate();
    _param_count(count);
    _write_buffer(buffer);
  }

  /// @brief default ctor
  /// @param buffer buffer data which it will be copied to the buffer
  /// @param length buffer length
  /// @param count params count
  Buffer(const char *buffer, size_t length, int count = 0) {
    iParams = count;

    _init(length, count);  // init the buffer
    _allocate();           // allocate the memory
    _param_count(count);
    _write_buffer((unsigned char *)buffer,
                  length);  // copy the buffer data from the original positions
  }

  Buffer(const Buffer &other) {
    //		std::cout << "shadow Copy ctor" << std::endl;

    _init(other.buffer_size(), (int)other.iParams);
    _allocate();
    _param_count((int)other.iParams);

    memcpy((void *)(raw), other.raw, length);
  }

  // deep copy assignment
  Buffer(Buffer &other) {
    std::cout << "Deep Copy ctor" << std::endl;
    // totally copy the original buffer
    {
      this->raw = other.raw;  // the raw data don't need to move just transfer
                              // the pointer's ownership
      this->length = other.length;
      this->iParams = other.iParams;
      this->count_offset = other.count_offset;
      this->key_offset = other.key_offset;
      this->value_offset = other.value_offset;
      this->buffer_offset = other.buffer_offset;
    }
    // remove the original buffer data
    {
      other.raw = nullptr;
      other.length = 0;
      other.iParams = 0;
      other.count_offset = 0;
      other.key_offset = 0;
      other.value_offset = 0;
      other.buffer_offset = 0;
    }
  }

  virtual ~Buffer() { _free(); }

 public:
  [[nodiscard]] bool IsModified() const { return (bool)raw[0]; }

  [[nodiscard]] int param_count() const { return *(int *)(raw + count_offset); }

  ParamType keys(size_t idx_) {
    if (idx_ < param_count())
      return *(ParamType *)(raw + key_offset + idx_ * p_size);
    return ParamType();
  }

  ParamType values(size_t idx_) {
    if (idx_ < param_count())
      return *(ParamType *)(raw + value_offset + idx_ * p_size);
    return ParamType();
  }

  [[nodiscard]] char *data() const { return (char *)(raw + buffer_offset); }

  void set_key(const int &idx_, const ParamType &key) const {
    if (idx_ < param_count())
      memcpy(static_cast<void *>(raw + key_offset + idx_ * p_size), &key,
             p_size);
  }

  void set_value(const int &idx_, const ParamType &value) const {
    if (idx_ < param_count())
      memcpy((void *)(raw + value_offset + idx_ * p_size), &value, p_size);
  }

  void set(const int &idx_, const ParamType &key,
           const ParamType &value) const {
    if (idx_ < param_count()) {
      set_key(idx_, key);
      set_value(idx_, value);
    }
  }

  [[nodiscard]] size_t size() const { return length; }

  /// @brief Get the real data buffer size
  /// @return the real buffer size to pass
  [[nodiscard]] size_t buffer_size() const {
    return length - buffer_offset - 1;
  }

 protected:
  /// @brief set buffer base info include length and each offsets
  /// @param buffer_len buffer data size
  /// @param count if need append paras count
  void _init(size_t buffer_len, int count = 0) {
    length = sizeof(bool) +    // modified
             sizeof(int) +     // count
             p_size * count +  // keys
             p_size * count +  // values
             buffer_len + 1;   // buffer
    count_offset = sizeof(bool);
    key_offset = count_offset + sizeof(int);
    value_offset = key_offset + p_size * count;
    buffer_offset = value_offset + p_size * count;
  }

  /// @brief allocate memory for the buffer
  void _allocate() {
    raw = (unsigned char *)Allocator::my_malloc(length);
    memset(raw, 0, length);
  }

  /// @brief released buffer from the memory
  void _free() const {
    if (raw == nullptr) return;  // if raw is null then it is already freed
    Allocator::my_free(raw);
  }

  /// @brief set the params count
  /// @param count params count
  void _param_count(int count = 0) const {
    if (count > 0) memset(raw, 1, sizeof(bool));
    memcpy((void *)(raw + count_offset), &count, sizeof(int));
  }

  /// @brief write buffer data to the buffer
  /// @param buffer buffer data as std::vector<char>
  void _write_buffer(std::vector<char> &buffer) const {
    //		memcpy((void *) (raw + buffer_offset), buffer.data(),
    // buffer.size());
    _write_buffer((unsigned char *)buffer.data(), buffer.size());
  }

  /// @brief write buffer data to the buffer
  /// @param buffer 	buffer data
  /// @param size 	buffer length
  void _write_buffer(const unsigned char *buffer, size_t size) const {
    memcpy((void *)(raw + buffer_offset), buffer, size);
  }

 private:
  unsigned char *raw{};  // raw buffer
  // all members must large than 0 , so don't need to use int16_t or something
  // just like this
  uint32_t length{0};         // buffer length if length is 0 means raw is null
  uint32_t iParams{0};        // params count
  uint32_t count_offset{0};   // count offset
  uint32_t key_offset{0};     // key offset
  uint32_t value_offset{0};   // value offset
  uint32_t buffer_offset{0};  // buffer offset
};

class GBuffer {
 public:
  GBuffer(unsigned int size, void *pData, unsigned int uiFlag);

  GBuffer();

  /// @brief if use the default constructor , you must call this function to
  /// init the buffer
  /// @param size the buffer size
  /// @param pData buffer data
  /// @param uiFlag see more in glBufferData
  unsigned int Update(unsigned int size, void *pData, unsigned int uiFlag);

  unsigned int ForceUpdate(unsigned int size, void *pData, unsigned int uiFlag);
  virtual ~GBuffer();

 public:
  [[nodiscard]] unsigned int size() const { return size_; }

  [[nodiscard]] unsigned int handle() const { return uiHandle_; }

  [[nodiscard]] unsigned int flag() const { return uiFlag_; }

 private:
  unsigned int size_{};
  unsigned int uiHandle_{};
  unsigned int uiFlag_{};
  bool bAllocated_{};
};

#pragma warning(pop)