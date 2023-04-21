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
    static char typeName[size] = {};
    memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);

    return typeName;
  }
};
}  // namespace internal

template <typename T>
const char *GetTypeName() {
  return internal::GetTypeNameHelper<T>::GetTypeName();
}

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
