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
constexpr auto kWindowHeight {768};
constexpr auto kSampleCount {1};

}

class Scene0 : public Scene {
public:
    Scene0() {
        mesh_ = Add(Mesh::Create(BoxGeometry::Create(), UnlitMaterial::Create(0xFF0000)));
    }

    auto OnUpdate(float delta) -> void override {
        mesh_->RotateX(1.0f * delta);
        mesh_->RotateY(1.0f * delta);
    }

private:
    Mesh* mesh_ {nullptr};
};

class Scene1 : public Scene {
public:
    Scene1(const std::shared_ptr<Texture2D>& texture) {
        auto material = UnlitMaterial::Create(0xFFFFFF);
        material->texture_map = texture;

        mesh_ = Add(Mesh::Create(BoxGeometry::Create(), material));
    }

    auto OnUpdate(float delta) -> void override {
        mesh_->RotateX(1.0f * delta);
        mesh_->RotateY(1.0f * delta);
    }

private:
    Mesh* mesh_ {nullptr};
};

auto make_scene_1(SharedContextPointer context) {
    auto scene = Scene::Create();
    scene->SetContext(context);
    return scene;
}

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
        .clear_color = Color {0.4f, 0.4f, 0.4f}
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

    camera->TranslateZ(3.0f);

    auto context = SharedContext::Create(&window, camera.get());

    auto target = RenderTarget::Create({
        .width = window.FramebufferWidth(),
        .height = window.FramebufferHeight(),
        .has_depth = true
    });

    auto scene_0 = std::make_shared<Scene0>();
    scene_0->SetContext(context.get());

    auto texture = renderer.CreateTextureView(target.get());
    auto scene_1 = std::make_shared<Scene1>(texture);
    scene_1->SetContext(context.get());

    auto timer = FrameTimer {true}; // auto-start
    while(!window.ShouldClose()) {
        window.PollEvents();
        window.BeginUIFrame();
        context->load_scheduler->Pump();
        const auto dt = timer.Tick();

        scene_0->Advance(dt);
        renderer.SetClearColor(0x000080);
        renderer.Render(scene_0.get(), camera.get(), target.get());

        scene_1->Advance(dt);
        renderer.SetClearColor(0x444444);
        renderer.Render(scene_1.get(), camera.get());

        window.EndUIFrame();
        window.SwapBuffers();
    }

    return 0;
}