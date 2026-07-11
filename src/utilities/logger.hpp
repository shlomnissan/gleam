/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx/core/identity.hpp"
#include "vglx/utilities/logging.hpp"

#include <format>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

namespace vglx {

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

            // std::format needs a compile-time string; std::vformat allows
            // runtime strings using format args.
            const auto message = std::vformat(
                format_str,
                std::make_format_args(static_cast<const Args&>(args)...)
            );

            Emit(level, message, loc);
        }

        Log(
            std::string_view format_str,
            Args&&... args,
            const std::source_location& loc = std::source_location::current()
        ) : Log(LogLevel::Debug, format_str, std::forward<Args>(args)..., loc) {}
    };

    template <typename... Args>
    struct LogOnce {
        LogOnce(
            LogLevel level,
            std::string_view format_str,
            Args&&... args,
            const std::source_location& loc = std::source_location::current()
        ) {
            if (std::to_underlying(level) > std::to_underlying(GetLevel())) return;

            const auto message = std::vformat(
                format_str,
                std::make_format_args(static_cast<const Args&>(args)...)
            );

            EmitOnce(level, message, loc);
        }

        LogOnce(
            std::string_view format_str,
            Args&&... args,
            const std::source_location& loc = std::source_location::current()
        ) : LogOnce(LogLevel::Debug, format_str, std::forward<Args>(args)..., loc) {}
    };

    template <typename... Args>
    Log(LogLevel level, std::string_view message, Args&&...) -> Log<Args...>;

    template <typename... Args>
    Log(std::string_view message, Args&&...) -> Log<Args...>;

    template <typename... Args>
    LogOnce(LogLevel level, std::string_view message, Args&&...) -> LogOnce<Args...>;

    template <typename... Args>
    LogOnce(std::string_view message, Args&&...) -> LogOnce<Args...>;

    static auto GetLevel() -> LogLevel;

    static void SetLevel(LogLevel level);

private:
    static void Emit(
        LogLevel level,
        std::string_view message,
        const std::source_location& loc
    );

    static void EmitOnce(
        LogLevel level,
        const std::string& message,
        const std::source_location& loc
    );
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
