/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/core/identity.hpp"
#include "vglx/utilities/logging.hpp"
#include "vglx/utilities/timer.hpp"

#include <atomic>
#include <filesystem>
#include <format>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>
#include <utility>

namespace vglx {

namespace fs = std::filesystem;

class Logger {
public:
    template <typename... Args>
    struct Log {
        Log(
            LogLevel level,
            std::string_view format_str,
            Args&&... args,
            const std::source_location& loc = std::source_location::current()
        ) {
            if (std::to_underlying(level) > std::to_underlying(GetLevel())) return;

            const auto lock = std::scoped_lock(mutex_);

            auto stream = level == LogLevel::Error ? &std::cerr : &std::cout;
            const auto& path = fs::path{loc.file_name()};
            // std::format needs a compile-time string; std::vformat allows
            // runtime strings using format args.
            const auto message = std::vformat(
                format_str,
                std::make_format_args(static_cast<const Args&>(args)...)
            );

            *stream << std::format(
                "[{}]{}: {} -> {}:{}\n",
                Timer::GetTimestamp(),
                GetLogLevelString(level),
                message,
                path.filename().string(),
                loc.line()
            );
        }

        Log(
            std::string_view format_str,
            Args&&... args,
            const std::source_location& loc = std::source_location::current()
        ) : Log(LogLevel::Debug, format_str, std::forward<Args>(args)..., loc) {}
    };

    template <typename... Args>
    Log(LogLevel level, std::string_view message, Args&&...) -> Log<Args...>;

    template <typename... Args>
    Log(std::string_view message, Args&&...) -> Log<Args...>;

    static auto GetLevel() -> LogLevel;

    static void SetLevel(LogLevel level);

private:
    static std::mutex mutex_;
    static std::atomic<LogLevel> runtime_level_;
};

}

namespace std {

template <typename T>
struct formatter<T, enable_if_t<std::is_base_of_v<vglx::Identity, T>, char>> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(const T& obj, FormatContext& ctx) const {
        return obj.Name().empty() ?
            std::format_to(ctx.out(), "[UUID: {}]", obj.UUID()) :
            std::format_to(ctx.out(), "[Name: {}]", obj.Name());
    }
};

}