#include "Logger.hpp"

#include <iostream>
std::unordered_map<std::string, std::shared_ptr<spdlog::logger>>
    Logger::m_loggers;
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> Logger::console_sink =
    std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
std::shared_ptr<spdlog::sinks::basic_file_sink_mt> Logger::file_sink =
    std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/monkey.log",
                                                        true);
std::shared_ptr<spdlog::sinks::basic_file_sink_mt> Logger::error_sink =
    std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/monkey_error.log",
                                                        true);
bool Logger::bInit = false;

std::shared_ptr<spdlog::logger> Logger::get(const std::string &name) {
  {
    if (!bInit) {
      console_sink->set_level(spdlog::level::trace);
      file_sink->set_level(spdlog::level::trace);
      error_sink->set_level(spdlog::level::err);
      bInit = true;
    }
    if (m_loggers.find(name) == m_loggers.end()) {
      auto logger = spdlog::logger(name, {console_sink, file_sink, error_sink});
      logger.set_level(spdlog::level::trace);
      logger.set_pattern("[%H:%M:%S.%e][%l] [%n]  %v");
      m_loggers[name] = std::make_shared<spdlog::logger>(logger);
    }
    return m_loggers[name];
  }
}
