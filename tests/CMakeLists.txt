find_package(GTest QUIET)

if (NOT GTest_FOUND)
    include(FetchContent)
    message(STATUS "📦 GTest not found, fetching via FetchContent...")
    set(INSTALL_GMOCK ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

file(GLOB TEST_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/**/**/*.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_LIST_DIR})

foreach(TEST IN LISTS TEST_SOURCES)
    get_filename_component(FILE_NAME ${TEST} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" NAME_NO_EXT ${FILE_NAME})
    message(STATUS "🧪 Adding test ${FILE_NAME}")

    set(TEST_TARGET run_${NAME_NO_EXT})
    add_executable(${TEST_TARGET} test_helpers.hpp ${TEST})

    target_compile_definitions(${TEST_TARGET} PRIVATE
        ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
    )

    target_link_libraries(${TEST_TARGET} PRIVATE GTest::gtest_main GTest::gmock vglx)
    add_test(${NAME_NO_EXT} ${TEST_TARGET})
endforeach()