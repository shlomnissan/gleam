/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <vglx/vglx.hpp>

#include <print>
#include <set>

using namespace vglx;

namespace {

constexpr auto kWindowWidth {1024};
constexpr auto kWindowHeight {1024};
constexpr auto kSampleCount {1};

constexpr auto vert_str = R"(
#version 410 core
#pragma inject_attributes

#include "snippets/vert_global_params.glsl"

void main() {
    #include "snippets/vert_main_varyings.glsl"
    gl_Position = u_Projection * v_Position;
})";

constexpr auto frag_str = R"(
#version 410 core
#pragma inject_attributes

layout(location = 0) out uvec4 v_FragColor;

in vec2 v_TexCoord;

const vec2 u_VirtualSize = vec2(8192.0, 8192.0);
const vec2 u_PageGrid = vec2(16.0, 16.0);
const vec2 u_MinMaxMipLevel = vec2(0.0, 4.0);
const float u_BufferScreenRatio = 0.25;

const uint VALID_BIT = 1u << 31;
const uint MIP_MASK  = 0x1Fu;
const uint PAGE_MASK = 0xFFu;

uint PackPageData(in uint mip, in uint page_x, in uint page_y) {
    return VALID_BIT |
          (mip & MIP_MASK) |
          ((page_x & PAGE_MASK) << 5) |
          ((page_y & PAGE_MASK) << 13);
}

float ComputeMipLevel(in vec2 effective_size, in vec2 uv) {
    vec2 dx = dFdx(uv) * effective_size;
    vec2 dy = dFdy(uv) * effective_size;
    float texel_footprint = max(dot(dx, dx), dot(dy, dy));
    return 0.5 * log2(max(texel_footprint, 1e-8));
}

void main() {
    vec2 effective_size = u_VirtualSize * u_BufferScreenRatio;

    uint mip_level = uint(clamp(
        ComputeMipLevel(effective_size, v_TexCoord),
        u_MinMaxMipLevel.x,
        u_MinMaxMipLevel.y
    ));

    float mip_scale = exp2(-float(mip_level));
    vec2 curr_page_grid = max(u_PageGrid * mip_scale, vec2(1.0));

    vec2 page_coords = floor(v_TexCoord * curr_page_grid);
    page_coords.y = (curr_page_grid.y - 1) - page_coords.y;
    page_coords = clamp(page_coords, vec2(0.0), curr_page_grid - 1.0);

    uint data = PackPageData(mip_level, uint(page_coords.x), uint(page_coords.y));

    v_FragColor = uvec4(data, 0, 0, 0);
})";

struct PageRequest {
    uint32_t lod;
    int x;
    int y;
    auto operator<=>(const PageRequest&) const = default;
};

}

class MainScene : public Scene {
public:
    MainScene() {
        mesh_ = Add(Mesh::Create(
            PlaneGeometry::Create({.width = 10.0f, .height = 10.0f}),
            default_material_
        ));
    }

    auto SetFeedbackPass(bool enabled) -> void {
        if (enabled) {
            mesh_->SetMaterial(feedback_material_);
        } else {
            mesh_->SetMaterial(default_material_);
        }
    }

private:
    Mesh* mesh_ {nullptr};

    std::shared_ptr<UnlitMaterial>
    default_material_ = {UnlitMaterial::Create(0xFFFFFF)};

    std::shared_ptr<ShaderMaterial>
    feedback_material_ = {ShaderMaterial::Create({vert_str, frag_str})};
};

auto main() -> int {
    auto window = Window {{
        .title = "Render Target Test",
        .width = kWindowWidth,
        .height = kWindowHeight,
        .sample_count = kSampleCount,
        .vsync = false
    }};

    auto init_window = window.Initialize();
    if (!init_window.has_value()) {
        std::println(stderr, "{}", init_window.error());
        return 1;
    }

    auto renderer = Renderer {{
        .framebuffer_width = window.FramebufferWidth(),
        .framebuffer_height = window.FramebufferHeight(),
        .sample_count = kSampleCount,
        .clear_color = {0x000000}
    }};

    auto init_renderer = renderer.Initialize();
    if (!init_renderer.has_value()) {
        std::println(stderr, "{}", init_renderer.error());
        return 1;
    }

    auto camera = PerspectiveCamera::Create({
        .fov = vglx::math::DegToRad(60.0f),
        .aspect = window.AspectRatio(),
        .near = 0.1f,
        .far = 1000.0f
    });

    auto context = SharedContext::Create(&window, camera.get());

    auto target = RenderTarget::Create({
        .width = window.FramebufferWidth() / 4,
        .height = window.FramebufferHeight() / 4,
        .format = Texture::Format::R32UI,
        .has_depth = true,
        .enable_readback = true
    });

    auto scene = std::make_shared<MainScene>();
    scene->SetContext(context.get());
    scene->Add(OrbitControls::Create(camera.get(), {.radius = 10.0f}));

    std::set<PageRequest> requests {};

    auto timer = FrameTimer {true}; // auto-start
    while(!window.ShouldClose()) {
        window.PollEvents();
        window.BeginUIFrame();
        context->load_scheduler->Pump();
        const auto dt = timer.Tick();

        scene->Advance(dt);
        scene->SetFeedbackPass(true);
        renderer.Render(scene.get(), camera.get(), target.get());

        auto data = target->ReadColorData();
        const auto pixel_count = data.size() / sizeof(std::uint32_t);
        auto pixels = std::span<const std::uint32_t>(
            reinterpret_cast<const std::uint32_t*>(data.data()),
            pixel_count
        );

        for (std::uint32_t packed : pixels) {
            if ((packed & (1u << 31)) == 0u) continue; // empty

            packed &= ~(1u << 31); // strip valid bit

            requests.emplace(
                packed & 0x1Fu,
                static_cast<int>((packed >> 5)  & 0xFFu),
                static_cast<int>((packed >> 13) & 0xFFu)
            );
        }

        // TODO: ingest feedback data

        scene->Advance(dt);
        scene->SetFeedbackPass(false);
        renderer.Render(scene.get(), camera.get());

        window.EndUIFrame();
        window.SwapBuffers();
    }

    return 0;
}