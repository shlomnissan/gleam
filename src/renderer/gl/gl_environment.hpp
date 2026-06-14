/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <glad/glad.h>

#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace vglx {

class GLProgram;
class Texture;

struct GLEnvironmentMaps {
    int base_size {0};
    int prefiltered_mips {0};
    GLuint base_cube {0};
    GLuint irradiance {0};
    GLuint prefiltered {0};
};

class GLEnvironment {
public:
    GLEnvironment() = default;

    // Non-copyable
    GLEnvironment(const GLEnvironment&) = delete;
    auto operator=(const GLEnvironment&) -> GLEnvironment& = delete;

    // Non-moveable
    GLEnvironment(GLEnvironment&&) = delete;
    auto operator=(GLEnvironment&&) -> GLEnvironment& = delete;

    auto Initialize() -> std::expected<void, std::string>;

    auto GetOrProcess(const std::shared_ptr<Texture>& source) -> std::optional<GLEnvironmentMaps>;

    ~GLEnvironment();

private:
    auto Process(const std::shared_ptr<Texture>& source) -> GLEnvironmentMaps;

    auto EquirectToCube(GLuint src_equirect, GLuint dst_cube, int size) -> void;

    std::unique_ptr<GLProgram> equirect_to_cube_;

    std::vector<std::pair<Texture*, GLEnvironmentMaps>> cache_;

    GLuint fbo_ {0};
    GLuint vao_ {0};
};

}