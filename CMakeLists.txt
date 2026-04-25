cmake_minimum_required(VERSION 3.20)

project(vglx VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINE_HIDDEN ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(VGLX_LOG_LEVEL "3" CACHE STRING "Default VGLX log level (0=Error, 1=Warning, 2=Info, 3=Debug)")

option(VGLX_BUILD_DOCS "Build API documentation using Doxygen" OFF)
option(VGLX_BUILD_EXAMPLES "Build example application" ON)
option(VGLX_BUILD_IMGUI "Build and integrate ImGui from the vendored source" ON)
option(VGLX_BUILD_TESTS "Build unit tests and test infrastructure using GTest" ON)

add_subdirectory("src")

if (VGLX_BUILD_TESTS)
    include(CTest)
    add_subdirectory("tests")
endif()

if (VGLX_BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()

if (VGLX_BUILD_DOCS)
    include(Doxygen)
endif()
