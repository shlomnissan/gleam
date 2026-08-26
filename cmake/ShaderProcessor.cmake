# ShaderProcessor.cmake
#
# This module searches for GLSL files and converts them into C-style strings.
# It makes a distinction between shaders and includes. Includes are used for
# common code that is included across multiple shaders at runtime.

file(GLOB_RECURSE SHADERS CONFIGURE_DEPENDS "**/*.vert" "**/*.frag" "**/*.glsl")

foreach(SHADER IN LISTS SHADERS)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SHADER})

    get_filename_component(FILENAME ${SHADER} NAME)
    get_filename_component(DIRECTORY ${SHADER} DIRECTORY)
    get_filename_component(EXTENSION ${SHADER} EXT)

    string(REGEX REPLACE "\\." "_" EXT ${EXTENSION})
    get_filename_component(FILENAME_NO_EXT ${SHADER} NAME_WE)
    set(HEADER_FILE ${DIRECTORY}/headers/${FILENAME_NO_EXT}${EXT}.h)

    get_filename_component(DIRECTORY_NAME ${DIRECTORY} NAME)
    if (DIRECTORY_NAME STREQUAL "include")
        set(VAR "_INCLUDE_${FILENAME_NO_EXT}")
        message("🎨 Writing include ${FILENAME_NO_EXT}.h")
    else()
        set(VAR "_SHADER_${FILENAME_NO_EXT}${EXT}")
        message("🎨 Writing shader ${FILENAME_NO_EXT}.h")
    endif()

    file(READ ${SHADER} CONTENTS)
    file(WRITE ${HEADER_FILE} "#pragma once\n\nstatic const char* ${VAR} = R\"(\n")
    file(APPEND ${HEADER_FILE} "${CONTENTS}")
    file(APPEND ${HEADER_FILE} "\n)\";")
endforeach()