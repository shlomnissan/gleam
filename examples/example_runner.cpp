/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_runner.hpp"

#include <print>
#include <string>

auto RunExample(vglx::Scene* scene, vglx::Camera* camera, std::string_view window_title) -> int {
    auto window = vglx::Window {{
        .title = std::string(window_title),
        .width = kWindowWidth,
        .height = kWindowHeight,
        .sample_count = kSampleCount,
        .vsync = true
    }};

    if (auto result = window.Initialize(); !result.has_value()) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    auto renderer = vglx::Renderer {{
        .framebuffer_width = window.FramebufferWidth(),
        .framebuffer_height = window.FramebufferHeight(),
        .sample_count = kSampleCount,
        .clear_color = 0x000000
    }};

    if (auto result = renderer.Initialize(); !result.has_value()) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    window.OnResize([&](const vglx::ResizeParameters& params){
        renderer.SetViewport(
            0, 0,
            params.framebuffer_width,
            params.framebuffer_height
        );
        camera->Resize(params.window_width, params.window_height);
    });

    auto timer = vglx::FrameTimer {true};

    while(!window.ShouldClose()) {
        window.PollEvents();
        scene->Advance(timer.Tick());
        renderer.Render(scene, camera);
        window.SwapBuffers();
    }

    return 0;
}