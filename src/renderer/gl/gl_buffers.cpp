/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_buffers.hpp"

#include "vglx/math/vector4.hpp"

#include "nodes/instanced_mesh_impl.hpp"
#include "utilities/logger.hpp"

#include <cstdint>
#include <utility>

namespace vglx {

namespace {

constexpr uint8_t BUFF_IDX_VBO  = 0;
constexpr uint8_t BUFF_IDX_EBO  = 1;
constexpr uint8_t BUFF_IDX_INSTANCE_COLOR = 2;
constexpr uint8_t BUFF_IDX_INSTANCE_TRANSFORM = 3;

}

#define BUFFER_OFFSET(offset) ((void*)(offset * sizeof(GLfloat)))

auto GLBuffers::Bind(const std::shared_ptr<Geometry>& geometry) -> void {
    auto vao = geometry->renderer_id;
    if (vao != 0 && vao == current_vao_) return;

    if (vao == 0) {
        GenerateBuffers(geometry.get());
        vao = geometry->renderer_id;
        geometries_.emplace_back(geometry);
    }

    glBindVertexArray(vao);
    current_vao_ = vao;
}

auto GLBuffers::GenerateBuffers(Geometry* geometry) -> void {
    auto& vao = geometry->renderer_id;
    auto buffers = std::array<GLuint, 4> {};

    glGenVertexArrays(1, &vao);
    glBindVertexArray(geometry->renderer_id);
    glGenBuffers(buffers.size(), buffers.data());

    const auto& vertex = geometry->VertexData();
    glBindBuffer(GL_ARRAY_BUFFER, buffers[BUFF_IDX_VBO]);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertex.size() * sizeof(GLfloat),
        vertex.data(),
        GL_STATIC_DRAW
    );

    auto offset = 0;
    auto stride = 0;
    for (const auto& attr : geometry->Attributes()) {
        if (attr.type == Geometry::VertexAttributeType::None) continue;
        stride += attr.item_size;
    }

    for (const auto& attr : geometry->Attributes()) {
        if (attr.type == Geometry::VertexAttributeType::None) continue;
        auto loc = std::to_underlying(attr.type);
        glVertexAttribPointer(
            loc,
            attr.item_size,
            GL_FLOAT,
            GL_FALSE,
            stride * sizeof(GLfloat),
            BUFFER_OFFSET(offset)
        );
        glEnableVertexAttribArray(loc);
        offset += attr.item_size;
    }

    if (geometry->IndexData().size()) {
        const auto& index = geometry->IndexData();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[BUFF_IDX_EBO]);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            index.size() * sizeof(GLuint),
            index.data(),
            GL_STATIC_DRAW
        );
    }

    bindings_.try_emplace(vao, std::move(buffers));

    geometry->OnDispose([this](Disposable* target){
        const auto vao = static_cast<Geometry*>(target)->renderer_id;
        auto& buffers = this->bindings_[vao];
        glDeleteBuffers(buffers.size(), buffers.data());
        Logger::Log(LogLevel::Info, "Geometry buffer cleared {}", *static_cast<Geometry*>(target));
        this->bindings_.erase(vao);
    });
}

auto GLBuffers::BindInstancedMesh(InstancedMesh* mesh) -> void {
    if (mesh->impl_->transforms_buff_id == 0) {
        auto& buffers = bindings_[mesh->GetGeometry()->renderer_id];
        mesh->impl_->transforms_buff_id = buffers[BUFF_IDX_INSTANCE_TRANSFORM];
        glBindBuffer(GL_ARRAY_BUFFER, mesh->impl_->transforms_buff_id);

        for (auto i = 0; i < 4; ++i) {
            auto loc = std::to_underlying(Geometry::VertexAttributeType::InstanceTransform) + i;
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(
                loc,
                4,
                GL_FLOAT,
                GL_FALSE,
                4 * sizeof(Vector4),
                BUFFER_OFFSET(i * 4)
            );
            glVertexAttribDivisor(loc, 1);
        }
    }

    if (mesh->impl_->transforms_touched) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->impl_->transforms_buff_id);
        glBufferData(
            GL_ARRAY_BUFFER,
            mesh->transforms_.size() * 4 * sizeof(Vector4),
            mesh->transforms_.data(),
            GL_DYNAMIC_DRAW
        );
        mesh->impl_->transforms_touched = false;
    }

    if (mesh->impl_->colors_buff_id == 0) {
        auto& buffers = bindings_[mesh->GetGeometry()->renderer_id];
        mesh->impl_->colors_buff_id = buffers[BUFF_IDX_INSTANCE_COLOR];
        glBindBuffer(GL_ARRAY_BUFFER, mesh->impl_->colors_buff_id);

        const auto loc = std::to_underlying(Geometry::VertexAttributeType::InstanceColor);
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(
                loc,
                3,
                GL_FLOAT,
                GL_FALSE,
                3 * sizeof(GL_FLOAT),
                BUFFER_OFFSET(0)
        );
        glVertexAttribDivisor(loc, 1);
    }

    if (mesh->impl_->colors_touched) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->impl_->colors_buff_id);

        glBufferData(
            GL_ARRAY_BUFFER,
            mesh->colors_.size() * sizeof(Color),
            mesh->colors_.data(),
            GL_DYNAMIC_DRAW
        );
        mesh->impl_->colors_touched = false;
    }
}

GLBuffers::~GLBuffers() {
    for (const auto& geometry : geometries_) {
        if (auto g = geometry.lock()) g->Dispose();
    }
}

}