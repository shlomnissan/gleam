/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <utilities/logger.hpp>

#include <string>

using namespace std::string_literals;

class LoggerTest : public testing::Test {
protected:
    void SetUp() override {
        previous_level_ = vglx::GetLogLevel();
        vglx::SetLogLevel(vglx::LogLevel::Debug);
    }

    void TearDown() override {
        vglx::SetLogLevel(previous_level_);
    }

private:
    vglx::LogLevel previous_level_;
};

#pragma region Standard Logger

TEST_F(LoggerTest, LogInfo) {
    testing::internal::CaptureStdout();
    vglx::Logger::Log(vglx::LogLevel::Info, "info");
    auto output = testing::internal::GetCapturedStdout();

    EXPECT_THAT(output, ::testing::HasSubstr("\x1B[1;34m[Info]\x1B[0m: info"));
}

TEST_F(LoggerTest, LogWarning) {
    testing::internal::CaptureStdout();
    vglx::Logger::Log(vglx::LogLevel::Warning, "warning");
    auto output = testing::internal::GetCapturedStdout();

    EXPECT_THAT(output, ::testing::HasSubstr("\x1B[1;33m[Warning]\x1B[0m: warning"));
}

TEST_F(LoggerTest, LogError) {
    testing::internal::CaptureStderr();
    vglx::Logger::Log(vglx::LogLevel::Error, "error");
    auto output = testing::internal::GetCapturedStderr();

    EXPECT_THAT(output, ::testing::HasSubstr("\x1B[1;31m[Error]\x1B[0m: error"));
}

TEST_F(LoggerTest, LogDebug) {
    testing::internal::CaptureStdout();
    vglx::Logger::Log(vglx::LogLevel::Debug, "debug");
    auto output = testing::internal::GetCapturedStdout();

    EXPECT_THAT(output, ::testing::HasSubstr("\x1B[1;35m[Debug]\x1B[0m: debug"));
}

#pragma endregion

#pragma region Log once

TEST_F(LoggerTest, LogOnceSuppressesRepeatedMessages) {
    testing::internal::CaptureStdout();
    vglx::Logger::LogOnce(vglx::LogLevel::Info, "log once repeated");
    vglx::Logger::LogOnce(vglx::LogLevel::Info, "log once repeated");
    auto output = testing::internal::GetCapturedStdout();

    const auto first = output.find("log once repeated");
    EXPECT_NE(first, std::string::npos);
    EXPECT_EQ(output.find("log once repeated", first + 1), std::string::npos);
}

TEST_F(LoggerTest, LogOnceEmitsDistinctMessages) {
    testing::internal::CaptureStdout();
    vglx::Logger::LogOnce(vglx::LogLevel::Info, "log once resource {}", "a"s);
    vglx::Logger::LogOnce(vglx::LogLevel::Info, "log once resource {}", "b"s);
    auto output = testing::internal::GetCapturedStdout();

    EXPECT_THAT(output, ::testing::HasSubstr("log once resource a"));
    EXPECT_THAT(output, ::testing::HasSubstr("log once resource b"));
}

#pragma endregion

#pragma region String formatting

TEST_F(LoggerTest, StringFormatting) {
    testing::internal::CaptureStdout();

    auto version = "OpenGL ES 3.2 NVIDIA 560.94 initialized"s;
    vglx::Logger::Log(vglx::LogLevel::Info, "version {}", version);

    auto output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, ::testing::HasSubstr(
        "\x1B[1;34m[Info]\x1B[0m: version OpenGL ES 3.2 NVIDIA 560.94 initialized")
    );
}

#pragma endregion
