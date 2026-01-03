/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <cstdlib>
#include <print>
#include <source_location>
#include <string_view>

#if !defined(VGLX_ENABLE_ASSERTS)
    #if !defined(NDEBUG)
        #define VGLX_ENABLE_ASSERTS 1
    #else
        #define VGLX_ENABLE_ASSERTS 0
    #endif
#endif

namespace vglx {

[[noreturn]] inline auto AssertFail(
    std::string_view expr,
    std::string_view msg,
    const std::source_location& loc
) -> void {
    std::println(
        stderr,
        "[VGLX_ASSERT]: {} ({}) -> {}:{}",
        msg,
        expr,
        loc.file_name(),
        loc.line()
    );
    std::abort();
}

}

#if VGLX_ENABLE_ASSERTS
  #define VGLX_ASSERT(expr, msg) \
    do { \
      if (!(expr)) { \
        ::vglx::AssertFail(#expr, (msg), std::source_location::current()); \
      } \
    } while (0)
#else
  #define VGLX_ASSERT(expr, msg) ((void)0)
#endif

#if defined(__clang__) || defined(__GNUC__)
    #define VGLX_UNREACHABLE() \
        do { \
            VGLX_ASSERT(false, "Unreachable code reached"); \
            __builtin_unreachable(); \
        } while (0)
#elif defined(_MSC_VER)
    #define VGLX_UNREACHABLE() \
        do { \
            VGLX_ASSERT(false, "Unreachable code reached"); \
            __assume(false); \
        } while (0)
#else
    #define VGLX_UNREACHABLE() \
        do { \
            VGLX_ASSERT(false, "Unreachable code reached"); \
            std::abort(); \
        } while (0)
#endif