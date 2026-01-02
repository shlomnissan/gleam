/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_renderer_impl.hpp"

#include "vglx/materials/phong_material.hpp"
#include "vglx/materials/shader_material.hpp"
#include "vglx/materials/sprite_material.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/vector3.hpp"
#include "vglx/scene/fog.hpp"
#include "vglx/scene/instanced_mesh.hpp"
#include "vglx/scene/sprite.hpp"

#include "core/program_attributes.hpp"
#include "core/render_lists.hpp"
#include "utilities/logger.hpp"

#include <glad/glad.h>

namespace vglx {

Renderer::Impl::Impl(const Renderer::Parameters& params)
  : scene_buffer_({
        params.framebuffer_width,
        params.framebuffer_height,
        params.sample_count,
    }),
    params_(params),
    render_lists_(std::make_unique<RenderLists>())
{
    scene_buffer_.Init();

    state_.SetViewport(0, 0, params.framebuffer_width, params.framebuffer_height);
    state_.SetClearColor(params.clear_color);
}

auto Renderer::Impl::Initialize() -> std::expected<void, std::string> {
    // no-op, nothing to initialize
    return {};
}

auto Renderer::Impl::RenderObjects(Scene* scene, Camera* camera) -> void {
    camera_ubo_.Update(camera->projection_matrix, camera->view_matrix);

    for (auto renderable : render_lists_->Opaque()) {
        RenderObject(renderable, scene, camera);
    }

    if (!render_lists_->Transparent().empty()) state_.SetDepthMask(false);
    for (auto renderable : render_lists_->Transparent()) {
        RenderObject(renderable, scene, camera);
    }

    state_.SetDepthMask(true);

    rendered_objects_per_frame_ = rendered_objects_counter_;
    rendered_objects_counter_ = 0;
}

auto Renderer::Impl::RenderObject(Renderable* renderable, Scene* scene, Camera* camera) -> void {
    auto geometry = renderable->GetGeometry().get();
    auto material = renderable->GetMaterial().get();
    auto attrs = ProgramAttributes {renderable, {
        .directional = lights_.directional,
        .point = lights_.point,
        .spot = lights_.spot
    }, scene};

    auto program = programs_.GetProgram(attrs);
    if (!program->IsValid()) {
        return;
    }

    state_.ProcessMaterial(material);
    if (material->wireframe && Renderable::IsMeshType(renderable)) {
        const auto mesh = static_cast<Mesh*>(renderable);
        buffers_.Bind(mesh->GetWireframeGeometry());
        geometry = mesh->GetWireframeGeometry().get();
    } else {
        buffers_.Bind(renderable->GetGeometry());
    }

    SetUniforms(program, &attrs, renderable, camera, scene);

    state_.UseProgram(program->Id());
    program->UpdateUniforms();

    auto primitive = GL_TRIANGLES;
    if (geometry->primitive == Geometry::PrimitiveType::Lines) {
        primitive = GL_LINES;
    }
    if (geometry->primitive == Geometry::PrimitiveType::LineLoop) {
        primitive = GL_LINE_LOOP;
    }

    const auto index_size = geometry->IndexData().size();
    const auto vertex_size = geometry->VertexCount();

    if (renderable->GetNodeType() != Node::Type::InstancedMesh) {
        index_size
            ? glDrawElements(primitive, index_size, GL_UNSIGNED_INT, nullptr)
            : glDrawArrays(primitive, 0, vertex_size);
    }

    if (renderable->GetNodeType() == Node::Type::InstancedMesh) {
        const auto instanced = static_cast<InstancedMesh*>(renderable);
        const auto count = instanced->Count();
        buffers_.BindInstancedMesh(instanced);

        index_size
            ? glDrawElementsInstanced(primitive, index_size, GL_UNSIGNED_INT, nullptr, count)
            : glDrawArraysInstanced(primitive, 0, vertex_size, count);
    }

    rendered_objects_counter_++;
}

auto Renderer::Impl::SetUniforms(
    GLProgram* program,
    ProgramAttributes* attrs,
    Renderable* renderable,
    Camera* camera,
    Scene* scene
) -> void {
    auto material = renderable->GetMaterial().get();
    auto model = renderable->GetWorldTransform();
    auto resolution = Vector2(
        params_.framebuffer_width,
        params_.framebuffer_height
    );

    program->SetUniform(Uniform::Model, &model);
    program->SetUniform(Uniform::Opacity, &material->opacity);
    program->SetUniform(Uniform::Resolution, &resolution);

    const auto bind_texture = [&](GLTextureMapType type, std::shared_ptr<Texture2D> tex) {
        textures_.Bind(tex, type);
        const auto& transform = tex->GetTransform();
        program->SetUniform(Uniform::TextureTransform, &transform);
        switch(type) {
            case GLTextureMapType::AlbedoMap:
                program->SetUniform(Uniform::AlbedoMap, &type);
                break;
            case GLTextureMapType::AlphaMap:
                program->SetUniform(Uniform::AlphaMap, &type);
                break;
            case GLTextureMapType::NormalMap:
                program->SetUniform(Uniform::NormalMap, &type);
                break;
            case GLTextureMapType::SpecularMap:
                program->SetUniform(Uniform::SpecularMap, &type);
                break;
            case GLTextureMapType::TextureMap:
                program->SetUniform(Uniform::TextureMap, &type);
                break;
            default:
                Logger::Log(LogLevel::Error, "Unable to bind unknown texture map type");
        }
    };

    if (auto fog = scene->fog.get()) {
        auto type = fog->GetType();
        program->SetUniform(Uniform::FogType, &type);
        if (type == Fog::Type::Linear) {
            program->SetUniform(Uniform::FogColor, &fog->color);
            program->SetUniform(Uniform::FogNear, &fog->near);
            program->SetUniform(Uniform::FogFar, &fog->far);
        }

        if (type == Fog::Type::Exponential) {
            program->SetUniform(Uniform::FogColor, &fog->color);
            program->SetUniform(Uniform::FogDensity, &fog->density);
        }
    }

    if (attrs->type == Material::Type::PhongMaterial) {
        auto m = static_cast<PhongMaterial*>(material);
        if (lights_.HasLights()) {
            program->SetUniform(Uniform::AmbientLight, &lights_.ambient_light);
            program->SetUniform(Uniform::MaterialDiffuseColor, &m->color);
            program->SetUniform(Uniform::MaterialSpecularColor, &m->specular);
            program->SetUniform(Uniform::MaterialShininess, &m->shininess);
        }

        if (attrs->albedo_map)
            bind_texture(GLTextureMapType::AlbedoMap, m->albedo_map);
        if (attrs->alpha_map)
            bind_texture(GLTextureMapType::AlphaMap, m->alpha_map);
        if (attrs->normal_map)
            bind_texture(GLTextureMapType::NormalMap, m->normal_map);
        if (attrs->specular_map)
            bind_texture(GLTextureMapType::SpecularMap, m->specular_map);
    }

    if (attrs->type == Material::Type::ShaderMaterial) {
        auto m = static_cast<ShaderMaterial*>(material);
        for (const auto& [name, value] : m->uniforms) {
            program->SetUnknownUniform(name, &value);
        }
    }

    if (attrs->type == Material::Type::SpriteMaterial) {
        auto m = static_cast<SpriteMaterial*>(material);
        auto r = static_cast<Sprite*>(renderable);

        program->SetUniform(Uniform::Anchor, &r->anchor);
        program->SetUniform(Uniform::Color, &m->color);
        program->SetUniform(Uniform::Rotation, &r->rotation);

        if (attrs->texture_map)
            bind_texture(GLTextureMapType::TextureMap, m->texture_map);
    }

    if (attrs->type == Material::Type::UnlitMaterial) {
        auto m = static_cast<UnlitMaterial*>(material);
        program->SetUniform(Uniform::Color, &m->color);

        if (attrs->texture_map)
            bind_texture(GLTextureMapType::TextureMap, m->texture_map);
        if (attrs->alpha_map)
            bind_texture(GLTextureMapType::AlphaMap, m->alpha_map);
    }
}

auto Renderer::Impl::ProcessLights(Camera* camera) -> void {
    lights_.Reset();

    for(auto light : render_lists_->Lights()) {
        lights_.AddLight(light, camera);
    }

    if (lights_.HasLights()) lights_.Update();
}

auto Renderer::Impl::Render(Scene* scene, Camera* camera) -> void {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene->UpdateTransformHierarchy();
    camera->UpdateViewMatrix();

    render_lists_->ProcessScene(scene, camera);
    ProcessLights(camera);

    RenderObjects(scene, camera);
}

auto Renderer::Impl::SetViewport(int x, int y, int width, int height) -> void {
    state_.SetViewport(x, y, width, height);
}

auto Renderer::Impl::SetClearColor(const Color& color) -> void {
    state_.SetClearColor(color);
}

Renderer::Impl::~Impl() = default;

}