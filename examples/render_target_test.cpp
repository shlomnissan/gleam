/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <vglx/vglx.hpp>

#include <print>

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

void main() {
    v_FragColor = uvec4(1, 0, 0, 0);
})";

}

class MainScene : public Scene {
public:
    MainScene() {
        mesh_ = Add(Mesh::Create(
            PlaneGeometry::Create({.width = 5.0f, .height = 5.0f}),
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

        scene->Advance(dt);
        scene->SetFeedbackPass(false);
        renderer.Render(scene.get(), camera.get());

        window.EndUIFrame();
        window.SwapBuffers();
    }

    return 0;
}