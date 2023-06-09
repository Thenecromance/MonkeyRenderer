#pragma once

#include <unordered_map>

#include "Patterns/SingleTon.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace internal {
static const unsigned int FRONT_SIZE =
    sizeof("internal::GetTypeNameHelper<") - 1u;
static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;

template <typename T>
struct GetTypeNameHelper {
  static const char *GetTypeName() {
    static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
    static char typeName[size] =
        {};  // ignore the error from the Logger::get<>() just because when
             // coding time the compiler can't know the size of the typeName,
             // but when the program is compiled, the size of the typeName is
             // known.
    memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);

    return typeName;
  }
};
}  // namespace internal

template <typename T>
const char *GetTypeName() {
  return internal::GetTypeNameHelper<T>::GetTypeName();
}

/// @brief a simple way to wrapped the spdlog
class Logger {
 public:
  static std::shared_ptr<spdlog::logger> get(const std::string &name);
  template <typename Type>
  static std::shared_ptr<spdlog::logger> get() {
    return get(GetTypeName<Type>());
  }

 private:
  static std::unordered_map<std::string, std::shared_ptr<spdlog::logger>>
      m_loggers;
  static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
  static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> file_sink;
  static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> error_sink;
  static bool bInit;
};
