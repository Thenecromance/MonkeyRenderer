
#pragma once

#include <cassert>

#ifdef NDEBUG
#define COMPILE_CHECK(v)
#define PTR_CHECK(v)
#define NONE_ZERO_CHECK(v)
#define PTR_ASSERT(v)
#define COMPILE_ASSERT(v)
#define RENDER_RUNTIME_ASSERT(v)
#else

#define COMPILE_ASSERT(description)                                        \
  auto err = glGetError();                                                 \
  if (err != GL_NO_ERROR) {                                                \
    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]:" << description \
              << "with error code:" << err << std::endl;                   \
    assert(false && description);                                          \
  }

#define PTR_ASSERT(ptr) assert(ptr != nullptr && #ptr "could not be null");

#define NONE_ZERO_CHECK(val) assert(val != 0 && #val "could not be 0 here");

#define RENDER_RUNTIME_ASSERT(description)                                 \
  auto err = glGetError();                                                 \
  if (err != GL_NO_ERROR) {                                                \
    std::cout << "[" << __FILE__ << ":" << __LINE__ << "]:" << description \
              << "with error code:" << err << std::endl;                   \
    assert(false && description);                                          \
  }
#endif
