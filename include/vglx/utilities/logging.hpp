/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "vglx_export.h"

#include <string>

namespace vglx {

/**
 * @brief Enumerates supported logging severity levels.
 */
enum class LogLevel {
    Error, ///< Critical errors that indicate a failure or invalid state.
    Warning, ///< Non-fatal issues that may indicate incorrect usage.
    Info, ///< high-level informational messages describing engine state.
    Debug ///< Diagnostic information intended for development and debugging.
};

/**
 * @brief Sets the active logging level for VGLX.
 *
 * Messages with a severity greater than the specified level
 * will be filtered out.
 *
 * @param level The maximum log level to emit.
 */
VGLX_EXPORT void SetLogLevel(LogLevel level);

/**
 * @brief Returns the currently active logging level.
 */
VGLX_EXPORT auto GetLogLevel() -> LogLevel;

/**
 * @brief Converts a @ref LogLevel value to a human-readable string.
 *
 * @param level Log level enum value.
 */
VGLX_EXPORT auto GetLogLevelString(LogLevel level) -> std::string;

}