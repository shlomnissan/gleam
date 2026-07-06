/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include "utilities/logger.hpp"

#include <string>
#include <chrono>

namespace vglx {

class ScopedTimer {
public:
    enum class Unit {
        Microseconds,
        Milliseconds
    };

    using Clock = std::chrono::high_resolution_clock;

    ScopedTimer(
        const std::string& label,
        Unit unit = Unit::Microseconds,
        LogLevel level = LogLevel::Debug
    ) : label_(label), unit_(unit), level_(level) {}

    auto Stop() {
        if (is_running_) {
            is_running_ = false;
            const auto end = Clock::now();
            const auto delta = end - start_;
            if (unit_ == Unit::Microseconds) {
                const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(delta);
                Logger::Log(level_, "{}: {} µs", label_, duration.count());
            }
            if (unit_ == Unit::Milliseconds) {
                const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta);
                Logger::Log(level_, "{}: {} ms", label_, duration.count());
            }
        }
    }

    ~ScopedTimer() {
        Stop();
    }

private:
    std::string label_;
    Unit unit_;
    LogLevel level_;
    Clock::time_point start_ {Clock::now()};
    bool is_running_ {true};
};

}