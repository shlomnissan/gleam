option(VGLX_EXAMPLES_ENABLE_UI "Enable the stats UI overlay in examples" ON)

add_library(example_runner STATIC example_runner.cpp example_runner.hpp)

target_include_directories(example_runner PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if (VGLX_EXAMPLES_ENABLE_UI)
    target_compile_definitions(example_runner PRIVATE VGLX_EXAMPLES_ENABLE_UI)
endif()

target_link_libraries(example_runner PUBLIC vglx)

file(GLOB SCENES ${CMAKE_CURRENT_LIST_DIR}/scenes/*.cpp)

foreach(SCENE IN LISTS SCENES)
    get_filename_component(FILE_NAME ${SCENE} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" SCENE_NAME ${FILE_NAME})
    message(STATUS "🧪 Adding scene ${SCENE_NAME}")

    set(TARGET example_${SCENE_NAME})

    add_executable(${TARGET} ${SCENE})

    target_compile_definitions(${TARGET} PRIVATE
        ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
    )

    target_link_libraries(${TARGET} PRIVATE example_runner vglx)
endforeach()
