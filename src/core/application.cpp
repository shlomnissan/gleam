/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/application.hpp"

#include "vglx/cameras/perspective_camera.hpp"
#include "vglx/core/renderer.hpp"
#include "vglx/core/shared_context.hpp"
#include "vglx/core/window.hpp"
#include "vglx/loaders/load_scheduler.hpp"
#include "vglx/utilities/frame_timer.hpp"
#include "vglx/utilities/stats.hpp"

#include "utilities/logger.hpp"

#include <expected>
#include <string>

namespace vglx {

namespace {

auto create_default_camera(int width, int height) {
    return PerspectiveCamera::Create({
        .fov = math::DegToRad(60.0f),
        .aspect = static_cast<float>(width) / static_cast<float>(height),
        .near = 0.1f,
        .far = 1000.0f
    });
}

}

struct Application::Impl {
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<SharedContext> context;
    std::unique_ptr<LoadScheduler> load_scheduler;

    double last_frame_time = 0.0;

    auto InitializeWindow(const Application::Parameters& params) -> std::expected<void, std::string> {
        window = std::make_unique<Window>(Window::Parameters{
            .title = params.title,
            .width = params.width,
            .height = params.height,
            .antialiasing = params.antialiasing,
            .vsync = params.vsync
        });
        return window->Initialize();
    }

    auto InitializeRenderer(const Application::Parameters& params) -> std::expected<void, std::string> {
        renderer = std::make_unique<Renderer>(Renderer::Parameters {
            .framebuffer_width = window->FramebufferWidth(),
            .framebuffer_height = window->FramebufferHeight(),
            .clear_color = params.clear_color
        });
        return renderer->Initialize();
    }

    auto MakeSharedContext() -> void {
        context = SharedContext::Create(
            window.get(),
            camera.get(),
            load_scheduler.get()
        );
    }

    auto SetCamera(std::unique_ptr<Camera> camera) -> void {
        this->camera = std::move(camera);
        if (!this->camera) {
            this->camera = create_default_camera(
                context->window_width,
                context->window_height
            );
        }
        context->camera = this->camera.get();
    }

    auto SetScene(std::unique_ptr<Scene> scene) -> void {
        this->scene = std::move(scene);
        this->scene->SetContext(context.get());
    }
};

Application::Application() : impl_(std::make_unique<Impl>()) {
    impl_->load_scheduler = std::make_unique<LoadScheduler>();
}

auto Application::Setup() -> void {
    const auto params = Configure();
    show_stats_ = params.show_stats;

    auto init_window_result = impl_->InitializeWindow(params);
    if (!init_window_result.has_value()) {
        Logger::Log(LogLevel::Error, "{}", init_window_result.error());
        return;
    }

    auto init_renderer_result = impl_->InitializeRenderer(params);
    if (!init_renderer_result.has_value()) {
        Logger::Log(LogLevel::Error, "{}", init_renderer_result.error());
        return;
    }

    impl_->MakeSharedContext();
    impl_->SetCamera(CreateCamera());
    impl_->SetScene(CreateScene());

    impl_->window->OnResize([this](const ResizeParameters& params){
        auto context = impl_->context.get();
        context->framebuffer_width = params.framebuffer_width;
        context->framebuffer_height = params.framebuffer_height;
        context->window_width = params.window_width;
        context->window_height = params.window_height;
        impl_->renderer->SetViewport(
            0, 0,
            params.framebuffer_width,
            params.framebuffer_height
        );
        impl_->camera->Resize(params.window_width, params.window_height);
    });
}

auto Application::Start() -> void {
    Setup();

    auto frame_timer = FrameTimer {true};
    auto stats = Stats {};

    while (!impl_->window->ShouldClose()) {
        impl_->window->PollEvents();
        impl_->load_scheduler->Pump();

        const auto dt = frame_timer.Tick();
        impl_->scene->Advance(dt);

        impl_->window->BeginUIFrame();
        if (!Update(dt)) {
            impl_->window->RequestClose();
        }
        if (show_stats_) {
            stats.Draw();
        }

        stats.BeforeRender();
        impl_->renderer->Render(impl_->scene.get(), impl_->camera.get());
        impl_->window->EndUIFrame();

        stats.AfterRender(impl_->renderer->RenderedObjectsPerFrame());
        impl_->window->SwapBuffers();
    }
}

auto Application::GetContext() const -> SharedContextPointer {
    return impl_->context.get();
}

auto Application::SetScene(std::unique_ptr<Scene> scene) -> void {
    impl_->SetScene(std::move(scene));
}

auto Application::SetCamera(std::unique_ptr<Camera> camera) -> void {
    impl_->SetCamera(std::move(camera));
}

Application::~Application() = default;

}