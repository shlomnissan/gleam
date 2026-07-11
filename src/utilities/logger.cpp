/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "utilities/logger.hpp"

#include "vglx/utilities/logging.hpp"
#include "vglx/utilities/timer.hpp"

#include <atomic>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_set>

#ifndef VGLX_LOG_LEVEL
    #define VGLX_LOG_LEVEL 3
#endif

namespace vglx {

constexpr LogLevel kDefaultLogLevel =
#if VGLX_LOG_LEVEL == 0
    LogLevel::Error;
#elif VGLX_LOG_LEVEL == 1
    LogLevel::Warning;
#elif VGLX_LOG_LEVEL == 2
    LogLevel::Info;
#elif VGLX_LOG_LEVEL == 3
    LogLevel::Debug;
#else
    #error "Invalid VGLX_LOG_LEVEL (must be 0..3)"
#endif
;

namespace fs = std::filesystem;

namespace {

std::atomic<LogLevel> runtime_level {kDefaultLogLevel};
std::mutex logger_mutex {};

}

void Logger::Emit(
    LogLevel level,
    std::string_view message,
    const std::source_location& loc
) {
    const auto lock = std::scoped_lock(logger_mutex);

    auto stream = level == LogLevel::Error ? &std::cerr : &std::cout;
    const auto path = fs::path {loc.file_name()};

    *stream << std::format(
        "[{}]{}: {} -> {}:{}\n",
        Timer::GetTimestamp(),
        GetLogLevelString(level),
        message,
        path.filename().string(),
        loc.line()
    );
}

void Logger::EmitOnce(
    LogLevel level,
    const std::string& message,
    const std::source_location& loc
) {
    static auto seen_messages = std::unordered_set<std::string>{};

    {
        const auto lock = std::scoped_lock(logger_mutex);
        if (!seen_messages.insert(message).second) return;
    }

    Emit(level, message, loc);
}

auto Logger::GetLevel() -> LogLevel {
    return runtime_level.load(std::memory_order_relaxed);
}

void Logger::SetLevel(LogLevel level) {
    runtime_level.store(level, std::memory_order_relaxed);
}

void SetLogLevel(LogLevel level) {
    Logger::SetLevel(level);
}

auto GetLogLevel() -> LogLevel {
    return Logger::GetLevel();
}

VGLX_EXPORT auto GetLogLevelString(LogLevel level) -> std::string {
    using enum LogLevel;
    switch (level) {
        case Error: return "\033[1;31m[Error]\033[0m";
        case Warning: return "\033[1;33m[Warning]\033[0m";
        case Info: return "\033[1;34m[Info]\033[0m";
        case Debug: return "\033[1;35m[Debug]\033[0m";
        default: return "Unknown";
    }
}

}