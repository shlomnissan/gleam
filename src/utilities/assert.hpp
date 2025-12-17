/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "utilities/logger.hpp"

#include <cstdlib>
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
    Logger::Log(LogLevel::Error, "{} ({})", msg, expr, loc);
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