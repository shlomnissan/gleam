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
#include "utilities/scoped_timer.hpp"

#include <glad/glad.h>

#include <utility>

namespace vglx {

Renderer::Impl::Impl(const Renderer::Parameters& params)
  : scene_buffer_({
        params.framebuffer_width,
        params.framebuffer_height,
        params.sample_count,
    }),
    viewport_width_(params.framebuffer_width),
    viewport_height_(params.framebuffer_height),
    render_lists_(std::make_unique<RenderLists>()),
    shadow_render_lists_(std::make_unique<RenderLists>()),
    shadow_map_(params.shadow_map),
    tone_mapping_(params.tone_mapping),
    exposure_(params.exposure)
{
    state_.SetViewport(0, 0, params.framebuffer_width, params.framebuffer_height);
    state_.SetClearColor(params.clear_color);
}

auto Renderer::Impl::Initialize() -> std::expected<void, std::string> {
#if !defined(NDEBUG)
    const auto timer = ScopedTimer(
        "Renderer initialization time",
        ScopedTimer::Unit::Milliseconds,
        LogLevel::Info
    );
#endif

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

    if (auto result = environment_.Initialize(); !result.has_value()) {
        return std::unexpected(result.error());
    }

    if (auto result = shadow_maps_.Initialize(); !result.has_value()) {
        return std::unexpected(result.error());
    }

    state_.SetDepthFunction(Material::Depth::LessEqual);
    state_.SetSeamlessCubemapFiltering();

    return {};
}

auto Renderer::Impl::RenderObjects(Scene* scene, Camera* camera) -> void {
    for (auto renderable : render_lists_->Opaque()) {
        RenderObject(renderable, scene, camera);
    }

    state_.SetDepthWrites(false);

    if (scene->background) {
        // Background draws at the far plane (clip-space z = 1.0), so it needs
        // LessEqual depth to pass against the cleared depth buffer. Cull-face
        // is off because a cube skybox is viewed from inside, where its
        // outward-wound faces would otherwise be culled.
        state_.SetDepthTest(true);
        state_.SetDepthFunction(Material::Depth::LessEqual);
        state_.SetSide(Material::Side::TwoSided);
        state_.SetBlending(Material::Blending::None);
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
        .spot = lights_.spot,
        .enable_shadow_maps = shadow_map_ != ShadowMap::None && lights_.has_shadow_casters,
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

    program->SetUniform(Uniform::Model, &model);
    program->SetUniform(Uniform::Opacity, &material->opacity);
    program->SetUniform(Uniform::Resolution, &resolution_);

    static const auto kIdentity = Matrix3::Identity();
    program->SetUniform(Uniform::TextureTransform, &kIdentity);

    const auto bind_texture = [&](GLTextureMapType type, std::shared_ptr<Texture> tex) {
        textures_.Bind(tex, std::to_underlying(type));
        if (tex->GetType() == Texture::Type::Texture2D) {
            const auto& transform = static_cast<Texture2D*>(tex.get())->transform.Get();
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
            case GLTextureMapType::EnvironmentMap:
                program->SetUniform(Uniform::EnvironmentMap, &type);
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

        if (attrs->ibl) {
            glActiveTexture(GL_TEXTURE0 + std::to_underlying(GLTextureMapType::IrradianceMap));
            glBindTexture(GL_TEXTURE_CUBE_MAP, env_maps_.irradiance);

            glActiveTexture(GL_TEXTURE0 + std::to_underlying(GLTextureMapType::PrefilteredMap));
            glBindTexture(GL_TEXTURE_CUBE_MAP, env_maps_.prefiltered);

            glActiveTexture(GL_TEXTURE0 + std::to_underlying(GLTextureMapType::BrdfLutMap));
            glBindTexture(GL_TEXTURE_2D, environment_.BrdfLut());

            const auto irradiance_unit = std::to_underlying(GLTextureMapType::IrradianceMap);
            const auto prefiltered_unit = std::to_underlying(GLTextureMapType::PrefilteredMap);
            const auto brdf_lut_unit = std::to_underlying(GLTextureMapType::BrdfLutMap);
            const auto prefiltered_max_lod = static_cast<float>(env_maps_.prefiltered_mips - 1);

            program->SetUniform(Uniform::IrradianceMap, &irradiance_unit);
            program->SetUniform(Uniform::PrefilteredMap, &prefiltered_unit);
            program->SetUniform(Uniform::BrdfLut, &brdf_lut_unit);
            program->SetUniform(Uniform::PrefilteredMaxLod, &prefiltered_max_lod);

            const auto env_intensity = scene->environment_intensity * m->environment_intensity;
            program->SetUniform(Uniform::EnvironmentIntensity, &env_intensity);
        }

        if (lights_.HasLights()) {
            program->SetUniform(Uniform::AmbientLight, &lights_.ambient_light);
            program->SetUniform(Uniform::MaterialColor, &m->color);
            program->SetUniform(Uniform::MaterialMetallic, &m->metallic);
            program->SetUniform(Uniform::MaterialRoughness, &m->roughness);

            if (attrs->shadow_maps) {
                auto tex_unit = std::to_underlying(GLTextureMapType::ShadowMap);
                glActiveTexture(GL_TEXTURE0 + tex_unit);
                glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_maps_.GetTexture2D());
                program->SetUniform(Uniform::ShadowMaps, &tex_unit);
                program->SetUniform(Uniform::ReceiveShadow, &renderable->receive_shadow);
            }
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

            if (attrs->shadow_maps) {
                auto tex_unit = std::to_underlying(GLTextureMapType::ShadowMap);
                glActiveTexture(GL_TEXTURE0 + tex_unit);
                glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_maps_.GetTexture2D());
                program->SetUniform(Uniform::ShadowMaps, &tex_unit);
                program->SetUniform(Uniform::ReceiveShadow, &renderable->receive_shadow);
            }
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
        if (attrs->environment_map) {
            bind_texture(GLTextureMapType::EnvironmentMap, m->environment_map);
            program->SetUniform(Uniform::Reflectivity, &m->reflectivity);
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
            program->SetUniform(name, &value);
        }
        for (const auto& [name, tex] : m->textures_) {
            const int tex_unit = kReservedTextureUnits + next_texture_unit_++;
            textures_.Bind(tex, tex_unit);
            program->SetUniform(name, &tex_unit);

            if (tex->GetType() == Texture::Type::Texture2D) {
                const auto& transform = static_cast<Texture2D*>(tex.get())->transform.Get();
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
        GLShadowMap* shadow_map {shadow_maps_.GetShadowMap(light)};
        if (shadow_map != nullptr) {
            lights_.AddLight(light, camera, &shadow_map->transform, shadow_map->map_idx);
        } else {
            lights_.AddLight(light, camera);
        }
    }

    if (lights_.HasLights()) lights_.Update();
}

auto Renderer::Impl::RenderShadowMaps(Scene* scene, Camera* camera) -> void {
    auto lights = std::vector<Light*> {};

    auto count_2d = 0u;
    auto count_cube = 0u;
    auto max_map_size_2d = 0u;
    auto max_map_size_cube = 0u;

    for (auto light : render_lists_->Lights()) {
        auto shadow = light->GetShadow();
        if (shadow == nullptr) {
            continue;
        }
        lights.emplace_back(light);
        if (light->GetType() == Light::Type::Point) {
            count_cube++;
            max_map_size_cube = std::max(max_map_size_cube, shadow->map_size);
        } else {
            count_2d++;
            max_map_size_2d = std::max(max_map_size_2d, shadow->map_size);
        }
    }

    auto result = shadow_maps_.StartFrame(
        count_2d,
        max_map_size_2d,
        count_cube,
        max_map_size_cube
    );

    if (!result.has_value()) {
        Logger::Log(LogLevel::Error, "{}", result.error());
        return;
    }

    for (auto light : lights) {
        auto shadow = light->GetShadow();
        auto result = shadow_maps_.BindShadowMap(light, camera);
        if (!result.has_value()) {
            Logger::Log(LogLevel::Error, "{}", result.error());
            continue;
        }

        auto camera = result.value();

        state_.SetViewport(0, 0, shadow->map_size, shadow->map_size);
        state_.SetDepthTest(true);
        state_.SetDepthWrites(true);

        glClear(GL_DEPTH_BUFFER_BIT);

        shadow_render_lists_->ProcessScene(scene, camera, false);
        camera_ubo_.Update(camera->projection_matrix, camera->view_matrix);

        for (auto renderable : shadow_render_lists_->Opaque()) {
            if (!renderable->cast_shadow) continue;

            auto geometry = renderable->GetGeometry();
            if (geometry->primitive != Geometry::PrimitiveType::Triangles) {
                continue;
            }

            auto is_instanced = renderable->GetNodeType() == Node::Type::InstancedMesh;
            auto program = shadow_maps_.GetProgram(is_instanced);
            auto model = renderable->GetWorldTransform();

            using enum Material::Side;
            switch (renderable->GetMaterial()->side) {
                case Front: state_.SetSide(Back); break;
                case Back: state_.SetSide(Front); break;
                case TwoSided: state_.SetSide(TwoSided); break;
            }

            state_.UseProgram(program->Id());

            program->SetUniform(Uniform::Model, &model);
            program->UpdateUniforms();

            const auto index_size = geometry->IndexData().size();
            const auto vertex_size = geometry->VertexCount();

            vertex_buffers_.Bind(geometry);

            if (is_instanced) {
                const auto instanced = static_cast<InstancedMesh*>(renderable);
                const auto count = instanced->Count();
                vertex_buffers_.BindInstancedMesh(instanced);
                index_size
                    ? glDrawElementsInstanced(GL_TRIANGLES, index_size, GL_UNSIGNED_INT, nullptr, count)
                    : glDrawArraysInstanced(GL_TRIANGLES, 0, vertex_size, count);
            } else {
                index_size
                    ? glDrawElements(GL_TRIANGLES, index_size, GL_UNSIGNED_INT, nullptr)
                    : glDrawArrays(GL_TRIANGLES, 0, vertex_size);
            }
        }
    }

    shadow_maps_.EndFrame();
}

auto Renderer::Impl::Render(Scene* scene, Camera* camera, RenderTarget* target) -> void {
    resolution_ = target != nullptr
        ? Vector2 { static_cast<float>(target->width), static_cast<float>(target->height) }
        : Vector2 { static_cast<float>(viewport_width_), static_cast<float>(viewport_height_) };

    if (scene->environment) {
        textures_.Bind(scene->environment, 0);
        auto env_maps = environment_.GetOrProcess(scene->environment);
        if (env_maps.has_value()) {
            env_maps_ = env_maps.value();
        }
    }

    scene->UpdateTransformHierarchy();
    camera->UpdateViewMatrix();

    render_lists_->ProcessScene(scene, camera);

    if (shadow_map_ != ShadowMap::None) {
        RenderShadowMaps(scene, camera);
    }

    const auto use_default_target = target == nullptr;
    use_default_target ? scene_buffer_.Begin() : framebuffers_.Begin(target);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera_ubo_.Update(camera->projection_matrix, camera->view_matrix);

    ProcessLights(camera);

    RenderObjects(scene, camera);

    use_default_target ? scene_buffer_.End() : framebuffers_.End(target);

    textures_.Reset();
    vertex_buffers_.Reset();
    state_.Reset();

    if (use_default_target) {
        present_pass_.Present(scene_buffer_, tone_mapping_, exposure_);
    }
}

auto Renderer::Impl::SetViewport(int x, int y, int width, int height) -> void {
    viewport_width_ = width;
    viewport_height_ = height;
    state_.SetViewport(x, y, width, height);
    scene_buffer_.ResizeViewport(width, height);
}

auto Renderer::Impl::SetClearColor(const Color& color) -> void {
    state_.SetClearColor(color);
}

auto Renderer::Impl::SetToneMapping(ToneMapping tone_mapping) -> void {
    tone_mapping_ = tone_mapping;
}

auto Renderer::Impl::SetExposure(float exposure) -> void {
    exposure_ = exposure;
}

auto Renderer::Impl::CreateTextureFromRenderTarget(RenderTarget* target) -> std::shared_ptr<Texture2D> {
    const auto tex_id = framebuffers_.GetColorAttachment(target);
    if (tex_id == 0) {
        Logger::Log(LogLevel::Error, "Failed to retrieve color attachment from target");
        return {};
    }

    auto texture = Texture2D::Create(Image::Create({
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
