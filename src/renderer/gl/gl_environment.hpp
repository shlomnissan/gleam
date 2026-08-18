/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#pragma once

#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <glad/glad.h>

namespace vglx {

class GLProgram;
class Texture;

struct GLEnvironmentMaps {
    int prefiltered_mips {0};
    GLuint base_cube {0};
    GLuint irradiance {0};
    GLuint prefiltered {0};
};

class GLEnvironment {
public:
    GLEnvironment() = default;

    GLEnvironment(const GLEnvironment&) = delete;
    GLEnvironment(GLEnvironment&&) = delete;

    auto operator=(const GLEnvironment&) -> GLEnvironment& = delete;
    auto operator=(GLEnvironment&&) -> GLEnvironment& = delete;

    auto Initialize() -> std::expected<void, std::string>;

    auto GetOrProcess(const std::shared_ptr<Texture>& source, GLuint texture_id) -> std::optional<GLEnvironmentMaps>;

    auto BrdfLut() const { return lut_; }

    ~GLEnvironment();

private:
    std::vector<std::pair<std::string, GLEnvironmentMaps>> cache_;

    std::shared_ptr<bool> alive_ { std::make_shared<bool>(true) };

    std::unique_ptr<GLProgram> prg_equirect_to_cube_;
    std::unique_ptr<GLProgram> prg_irradiance_cube_;
    std::unique_ptr<GLProgram> prg_prefiltered_cube_;
    std::unique_ptr<GLProgram> prg_brdf_lut_;

    GLuint fbo_ {0};
    GLuint vao_ {0};
    GLuint lut_ {0};

    auto RenderToCubeFaces(GLProgram* program, GLuint dst, int size, int mip = 0) -> void;

    auto EquirectToCubeMap(GLuint src, GLuint dst) -> void;

    auto IrradianceMap(GLuint src, GLuint dst) -> void;

    auto PrefilteredMap(GLuint src, GLuint dst) -> void;

    auto GenerateBrdfLut() -> void;
};

}
