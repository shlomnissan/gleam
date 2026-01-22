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

constexpr auto kWindowWidth {1024};
constexpr auto kWindowHeight {768};
constexpr auto kSampleCount {1};

}

auto main() -> int {
    auto window = Window {{
        .title = "Examples (Direct Initialization)",
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
        .clear_color = 0x444444
    }};

    auto init_renderer = renderer.Initialize();
    if (!init_renderer.has_value()) {
        std::println(stderr, "{}", init_renderer.error());
        return 1;
    }

    auto camera = PerspectiveCamera::Create({
        .fov = math::DegToRad(60.0f),
        .aspect = window.AspectRatio(),
        .near = 0.1f,
        .far = 1000.0f
    });

    auto context = SharedContext::Create(&window, camera.get());

    auto scene = std::unique_ptr<Scene> {};
    auto examples = Examples {[&scene, &context](std::unique_ptr<Scene> sc){
        scene = std::move(sc);
        scene->SetContext(context.get());
    }};

    scene = examples.GetScene();
    scene->SetContext(context.get());

    window.OnResize([&](const ResizeParameters& params){
        context->framebuffer_width = params.framebuffer_width;
        context->framebuffer_height = params.framebuffer_height;
        context->window_width = params.window_width;
        context->window_height = params.window_height;
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
        context->load_scheduler->Pump();

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