/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_sandbox.hpp"

#include <vglx/helpers.hpp>
#include <vglx/lights.hpp>
#include <vglx/materials.hpp>
#include <vglx/math.hpp>
#include <vglx/primitives.hpp>
#include <vglx/textures.hpp>

using namespace vglx;

namespace {

auto kTexWidth = 256;
auto kTexHeight = 256;

auto is_small_checker = false;
auto plane_geometry = PlaneGeometry::Create();
auto plane_material = PhongMaterial::Create();
auto texture = std::shared_ptr<DynamicTexture2D> ();

auto make_checker_rgba8(int w, int h, int check_size) -> std::vector<std::uint8_t>;

}

ExampleSandbox::ExampleSandbox() {
    show_context_menu_ = false;

    texture = DynamicTexture2D::Create({
        .width = kTexWidth,
        .height = kTexHeight,
        .mips = 1,
        .format = DynamicTexture2D::Format::RGBA8,
        .color_space = Texture::ColorSpace::Linear
    });
    auto bytes = make_checker_rgba8(kTexWidth, kTexHeight, 20);
    texture->UpdateSubregion(0, 0, 0, kTexWidth, kTexHeight, bytes);

    plane_material->albedo_map = texture;

    Add(PointLight::Create({0xFFFFFF, 1.0f}))->transform.Translate({0.5f, 0.5f, 0.8f});
    Add(Mesh::Create(plane_geometry, plane_material))->SetScale(2.0f);
}

auto ExampleSandbox::OnAttached(SharedContextPointer context) -> void {
    Add(OrbitControls::Create(
        context->camera, {
            .radius = 4.0f,
            .pitch = math::pi_over_6,
            .yaw = math::pi_over_6
        })
    );
}

auto ExampleSandbox::OnKeyboardEvent(vglx::KeyboardEvent* event) -> void {
    if (event->type == KeyboardEvent::Type::Pressed && event->key == Key::Space) {
        auto bytes = make_checker_rgba8(kTexWidth, kTexHeight, is_small_checker ? 20 : 5);
        texture->UpdateSubregion(0, 0, 0, kTexWidth, kTexHeight, bytes);
        is_small_checker = !is_small_checker;
    }
}

namespace {

auto make_checker_rgba8(int w, int h, int check_size) -> std::vector<std::uint8_t> {
    std::vector<std::uint8_t> out(static_cast<std::size_t>(w) * h * 4);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const bool even = ((x / check_size) % 2) == ((y / check_size) % 2);
            const std::uint8_t v = even ? 255 : 0;
            const std::size_t i = (static_cast<std::size_t>(y) * w + x) * 4;
            out[i + 0] = v;
            out[i + 1] = v;
            out[i + 2] = v;
            out[i + 3] = 255;
        }
    }
    return out;
}

}