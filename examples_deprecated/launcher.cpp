/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include <vglx/vglx.hpp>

#include "examples.hpp"

#include <print>

using namespace vglx;

namespace {

constexpr auto kWindowWidth {1280};
constexpr auto kWindowHeight {720};
constexpr auto kSampleCount {4};

}

auto main() -> int {
    auto window = Window {{
        .title = "Examples (Direct Initialization)",
        .width = kWindowWidth,
        .height = kWindowHeight,
        .vsync = true
    }};

    if (auto result = window.Initialize(); !result.has_value()) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    auto renderer = Renderer {{
        .framebuffer_width = window.FramebufferWidth(),
        .framebuffer_height = window.FramebufferHeight(),
        .sample_count = kSampleCount,
        .clear_color = 0x444444
    }};

    if (auto result = renderer.Initialize(); !result.has_value()) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    auto camera = PerspectiveCamera::Create({
        .fov = math::DegToRad(60.0f),
        .aspect = window.AspectRatio(),
        .near = 0.1f,
        .far = 1000.0f
    });

    auto scene = std::unique_ptr<Scene> {};
    auto examples = Examples {camera.get(), [&scene](std::unique_ptr<Scene> sc){
        scene = std::move(sc);
    }};

    scene = examples.GetScene();

    window.OnResize([&](const ResizeParameters& params){
        renderer.SetViewport(
            0, 0,
            params.framebuffer_width,
            params.framebuffer_height
        );
        camera->Resize(params.window_width, params.window_height);
    });

    auto timer = FrameTimer {true}; // auto-start
    auto stats = Stats {};

    while(!window.ShouldClose()) {
        window.PollEvents();

        const auto dt = timer.Tick();
        scene->Advance(dt);

        window.BeginUIFrame();
        examples.Draw();
        stats.Draw();

        stats.BeforeRender();
        renderer.Render(scene.get(), camera.get());
        window.EndUIFrame();

        stats.AfterRender(renderer.RenderedObjectsPerFrame());
        window.SwapBuffers();
    }

    return 0;
}