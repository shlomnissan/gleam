/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_renderer_impl.hpp"

#include "renderer/gl/gl_textures.hpp"
#include "vglx/core/render_target.hpp"
#include "vglx/materials/pbr_material.hpp"
#include "vglx/materials/phong_material.hpp"
#include "vglx/materials/shader_material.hpp"
#include "vglx/materials/sprite_material.hpp"
#include "vglx/materials/unlit_material.hpp"
#include "vglx/math/matrix3.hpp"
#include "vglx/scene/fog.hpp"
#include "vglx/scene/instanced_mesh.hpp"
#include "vglx/scene/sprite.hpp"
#include "vglx/textures/image.hpp"

#include "core/program_attributes.hpp"
#include "core/render_lists.hpp"
#include "utilities/logger.hpp"

#include <glad/glad.h>

#include <utility>

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
    state_.SetViewport(0, 0, params.framebuffer_width, params.framebuffer_height);
    state_.SetClearColor(params.clear_color);
}

auto Renderer::Impl::Initialize() -> std::expected<void, std::string> {
    const auto& info = gl::driver_info();
    Logger::Log(LogLevel::Info, "Vendor: {}", info.vendor);
    Logger::Log(LogLevel::Info, "Renderer: {}", info.renderer);
    Logger::Log(LogLevel::Info, "Version: {}", info.version);
    Logger::Log(LogLevel::Info, "GLSL Version: {}", info.glsl_version);

    if (auto result = scene_buffer_.Initialize(); !result.has_value()) {
        return std::unexpected(result.error());
    }

    if (auto result = present_pass_.Initialize(); !result.has_value()) {
        return std::unexpected(result.error());
    }

    if (auto result = background_pass_.Initialize(); !result.has_value()) {
        return std::unexpected(result.error());
    }

    state_.SetDepthFunction(Material::Depth::LessEqual);

    return {};
}

auto Renderer::Impl::RenderObjects(Scene* scene, Camera* camera) -> void {
    for (auto renderable : render_lists_->Opaque()) {
        RenderObject(renderable, scene, camera);
    }

    state_.SetDepthWrites(false);

    if (scene->background) {
        textures_.Bind(scene->background, 0);
        background_pass_.Render(scene->background);
    }

    if (!render_lists_->Transparent().empty())
    for (auto renderable : render_lists_->Transparent()) {
        RenderObject(renderable, scene, camera);
    }

    state_.SetDepthWrites(true);

    rendered_objects_per_frame_ = rendered_objects_counter_;
    rendered_objects_counter_ = 0;
    next_texture_unit_ = 0;
}

auto Renderer::Impl::RenderObject(Renderable* renderable, Scene* scene, Camera* camera) -> void {
    auto geometry = renderable->GetGeometry();
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
        geometry = mesh->GetWireframeGeometry();
        vertex_buffers_.Bind(geometry);
    } else {
        vertex_buffers_.Bind(geometry);
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
        vertex_buffers_.BindInstancedMesh(instanced);

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
        static_cast<float>(params_.framebuffer_width),
        static_cast<float>(params_.framebuffer_height)
    );

    program->SetUniform(Uniform::Model, &model);
    program->SetUniform(Uniform::Opacity, &material->opacity);
    program->SetUniform(Uniform::Resolution, &resolution);

    static const auto kIdentity = Matrix3::Identity();
    program->SetUniform(Uniform::TextureTransform, &kIdentity);

    const auto bind_texture = [&](GLTextureMapType type, std::shared_ptr<Texture> tex) {
        textures_.Bind(tex, std::to_underlying(type));
        if (tex->GetType() == Texture::Type::Texture2D) {
            const auto& transform = static_cast<Texture2D*>(tex.get())->GetTransform();
            program->SetUniform(Uniform::TextureTransform, &transform);
        }

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
            case GLTextureMapType::EmissiveMap:
                program->SetUniform(Uniform::EmissiveMap, &type);
                break;
            case GLTextureMapType::MetallicMap:
                program->SetUniform(Uniform::MetallicMap, &type);
                break;
            case GLTextureMapType::RoughnessMap:
                program->SetUniform(Uniform::RoughnessMap, &type);
                break;
            case GLTextureMapType::AOMap:
                program->SetUniform(Uniform::AOMap, &type);
                break;
            default:
                Logger::Log(LogLevel::Error, "Unable to bind unknown texture map type");
        }
    };

    if (scene->fog) {
        const auto& fog = scene->fog.value();
        program->SetUniform(Uniform::FogType, &fog.type);
        if (fog.type == Fog::Type::Linear) {
            program->SetUniform(Uniform::FogColor, &fog.color);
            program->SetUniform(Uniform::FogNear, &fog.near);
            program->SetUniform(Uniform::FogFar, &fog.far);
        }

        if (fog.type == Fog::Type::Exponential) {
            program->SetUniform(Uniform::FogColor, &fog.color);
            program->SetUniform(Uniform::FogDensity, &fog.density);
        }
    }

    if (attrs->type == Material::Type::PBRMaterial) {
        auto m = static_cast<PBRMaterial*>(material);
        if (lights_.HasLights()) {
            program->SetUniform(Uniform::AmbientLight, &lights_.ambient_light);
            program->SetUniform(Uniform::MaterialColor, &m->color);
            program->SetUniform(Uniform::MaterialMetallic, &m->metallic);
            program->SetUniform(Uniform::MaterialRoughness, &m->roughness);
        }

        program->SetUniform(Uniform::EmissiveColor, &m->emissive_color);
        program->SetUniform(Uniform::EmissiveIntensity, &m->emissive_intensity);

        if (attrs->albedo_map) {
            bind_texture(GLTextureMapType::AlbedoMap, m->albedo_map);
        }
        if (attrs->alpha_map) {
            bind_texture(GLTextureMapType::AlphaMap, m->alpha_map);
        }
        if (attrs->ao_map) {
            bind_texture(GLTextureMapType::AOMap, m->ao_map);
            program->SetUniform(Uniform::AOIntensity, &m->ao_intensity);
        }
        if (attrs->emissive_map) {
            bind_texture(GLTextureMapType::EmissiveMap, m->emissive_map);
        }
        if (attrs->normal_map) {
            bind_texture(GLTextureMapType::NormalMap, m->normal_map);
            program->SetUniform(Uniform::NormalIntensity, &m->normal_intensity);
        }
        if (attrs->metallic_map) {
            bind_texture(GLTextureMapType::MetallicMap, m->metallic_map);
        }
        if (attrs->roughness_map) {
            bind_texture(GLTextureMapType::RoughnessMap, m->roughness_map);
        }
    }

    if (attrs->type == Material::Type::PhongMaterial) {
        auto m = static_cast<PhongMaterial*>(material);
        if (lights_.HasLights()) {
            program->SetUniform(Uniform::AmbientLight, &lights_.ambient_light);
            program->SetUniform(Uniform::MaterialDiffuseColor, &m->color);
            program->SetUniform(Uniform::MaterialSpecularColor, &m->specular_color);
            program->SetUniform(Uniform::MaterialShininess, &m->shininess);
        }

        program->SetUniform(Uniform::EmissiveColor, &m->emissive_color);
        program->SetUniform(Uniform::EmissiveIntensity, &m->emissive_intensity);

        if (attrs->albedo_map) {
            bind_texture(GLTextureMapType::AlbedoMap, m->albedo_map);
        }
        if (attrs->alpha_map) {
            bind_texture(GLTextureMapType::AlphaMap, m->alpha_map);
        }
        if (attrs->ao_map) {
            bind_texture(GLTextureMapType::AOMap, m->ao_map);
            program->SetUniform(Uniform::AOIntensity, &m->ao_intensity);
        }
        if (attrs->emissive_map) {
            bind_texture(GLTextureMapType::EmissiveMap, m->emissive_map);
        }
        if (attrs->normal_map) {
            bind_texture(GLTextureMapType::NormalMap, m->normal_map);
            program->SetUniform(Uniform::NormalIntensity, &m->normal_intensity);
        }
        if (attrs->specular_map) {
            bind_texture(GLTextureMapType::SpecularMap, m->specular_map);
        }
    }

    if (attrs->type == Material::Type::ShaderMaterial) {
        auto m = static_cast<ShaderMaterial*>(material);
        for (const auto& [name, value] : m->uniforms_) {
            program->SetUnknownUniform(name, &value);
        }
        for (const auto& [name, tex] : m->textures_) {
            const int tex_unit = kReservedTextureUnits + next_texture_unit_++;
            textures_.Bind(tex, tex_unit);
            program->SetUnknownUniform(name, &tex_unit);

            if (tex->GetType() == Texture::Type::Texture2D) {
                const auto& transform = static_cast<Texture2D*>(tex.get())->GetTransform();
                program->SetUniform(Uniform::TextureTransform, &transform);
            }
        }
    }

    if (attrs->type == Material::Type::SpriteMaterial) {
        auto m = static_cast<SpriteMaterial*>(material);
        auto r = static_cast<Sprite*>(renderable);

        program->SetUniform(Uniform::Anchor, &r->anchor);
        program->SetUniform(Uniform::Color, &m->color);
        program->SetUniform(Uniform::Rotation, &r->rotation);

        if (attrs->texture_map) {
            bind_texture(GLTextureMapType::TextureMap, m->texture_map);
        }
    }

    if (attrs->type == Material::Type::UnlitMaterial) {
        auto m = static_cast<UnlitMaterial*>(material);
        program->SetUniform(Uniform::Color, &m->color);

        if (attrs->alpha_map) {
            bind_texture(GLTextureMapType::AlphaMap, m->alpha_map);
        }
        if (attrs->texture_map) {
            bind_texture(GLTextureMapType::TextureMap, m->texture_map);
        }
    }
}

auto Renderer::Impl::ProcessLights(Camera* camera) -> void {
    lights_.Reset();

    for(auto light : render_lists_->Lights()) {
        lights_.AddLight(light, camera);
    }

    if (lights_.HasLights()) lights_.Update();
}

auto Renderer::Impl::Render(Scene* scene, Camera* camera, RenderTarget* target) -> void {
    const auto use_default_target = target == nullptr;

    use_default_target ? scene_buffer_.Begin() : framebuffers_.Begin(target);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene->UpdateTransformHierarchy();
    camera->UpdateViewMatrix();

    camera_ubo_.Update(camera->projection_matrix, camera->view_matrix);
    render_lists_->ProcessScene(scene, camera);
    ProcessLights(camera);

    RenderObjects(scene, camera);

    use_default_target ? scene_buffer_.End() : framebuffers_.End(target);

    textures_.Reset();
    vertex_buffers_.Reset();
    state_.Reset();

    if (use_default_target) {
        present_pass_.Present(scene_buffer_);
    }
}

auto Renderer::Impl::SetViewport(int x, int y, int width, int height) -> void {
    state_.SetViewport(x, y, width, height);
    scene_buffer_.ResizeViewport(width, height);
}

auto Renderer::Impl::SetClearColor(const Color& color) -> void {
    state_.SetClearColor(color);
}

auto Renderer::Impl::CreateTextureFromRenderTarget(RenderTarget* target) -> std::shared_ptr<Texture2D> {
    const auto tex_id = framebuffers_.GetColorAttachment(target);
    if (tex_id == 0) {
        Logger::Log(LogLevel::Error, "Failed to retrieve color attachment from target");
        return {};
    }

    auto texture = Texture2D::Create(Image::Create({
        .data = {},
        .width = static_cast<unsigned int>(target->width),
        .height = static_cast<unsigned int>(target->height),
    }));

    texture->color_space = Texture::ColorSpace::Linear;
    texture->renderer_id = tex_id;
    texture->min_filter = Texture::MinFilter::Linear;
    texture->mag_filter = Texture::MagFilter::Linear;

    return texture;
}

Renderer::Impl::~Impl() = default;

}