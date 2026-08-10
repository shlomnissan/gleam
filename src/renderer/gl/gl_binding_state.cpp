/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_binding_state.hpp"

#include "vglx/geometries/buffer_attribute.hpp"

#include "utilities/logger.hpp"

#include <string>

#include <glad/glad.h>

namespace vglx {

namespace {

struct AttributeWithLocation {
    GLint location {0};
    std::shared_ptr<BufferAttribute> attribute {nullptr};
};

}

auto GLBindingState::Bind(Geometry2& geometry, const GLProgram& program) -> GLuint {
    auto error = [&geometry](std::string_view message) {
        auto name = geometry.Name().empty() ? geometry.UUID() : geometry.Name();
        Logger::Log(LogLevel::Error, "Failed to bind geometry {}. {}", name, message);
    };

    if (geometry.Disposed()) {
        error("Geometry is marked as disposed");
        return 0;
    }

    // TODO: check cache

    if (!geometry.HasPositions()) {
        error("Geometry doesn't contain vertex positions");
        return 0;
    }

    if (!program.IsValid()) {
        error("Program is invalid");
        return 0;
    }

    auto program_attributes = program.GetVertexAttributeLocations();
    if (program_attributes.empty()) {
        error("Program doesn't have any active vertex attributes");
        return 0;
    }

    auto attributes = std::vector<AttributeWithLocation> {};

    auto missing_attributes = std::string {};
    for (const auto& a : program_attributes) {
        auto attribute = geometry.GetAttribute(a.name);
        if (attribute == nullptr) {
            missing_attributes.append(a.name + ", ");
            continue;
        }
        attributes.emplace_back(a.location, attribute);
    }

    if (!missing_attributes.empty()) {
        missing_attributes.resize(missing_attributes.size() - 2);
        error("Missing program attributes " + missing_attributes);
        return 0;
    }

    // TODO: bind logic

    return 0;
}

auto GLBindingState::Bind(InstancedMesh2& instanced_mesh, const GLProgram& program) -> GLuint {
    // TODO: implement
    return 0;
}

GLBindingState::~GLBindingState() {
    // TODO: implement
}

}
