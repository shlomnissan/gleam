/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/core/window.hpp"

#include "core/window_impl.hpp"

namespace vglx {

Window::Window(const Window::Parameters& params)
  : impl_(std::make_unique<Impl>(params)) {}

Window::Window(Window&&) noexcept = default;

auto Window::operator=(Window&&) noexcept -> Window& = default;

auto Window::Initialize() -> std::expected<void, std::string> {
    return impl_->Initialize();
}

auto Window::PollEvents() -> void {
    impl_->PollEvents();
}

auto Window::BeginUIFrame() -> void {
    impl_->BeginUIFrame();
}

auto Window::EndUIFrame() -> void {
    impl_->EndUIFrame();
}

auto Window::SwapBuffers() -> void {
    impl_->SwapBuffers();
}

auto Window::RequestClose() -> void {
    impl_->RequestClose();
}

auto Window::ShouldClose() -> bool {
    return impl_->ShouldClose();
}

auto Window::FramebufferWidth() const -> int {
    return impl_->framebuffer_width;
}

auto Window::FramebufferHeight() const -> int {
    return impl_->framebuffer_height;
}

auto Window::Width() const -> int {
    return impl_->window_width;
}

auto Window::Height() const -> int {
    return impl_->window_height;
}

auto Window::AspectRatio() const -> float {
  return static_cast<float>(impl_->window_width) /
         static_cast<float>(impl_->window_height);
};

auto Window::SetTitle(std::string_view title) -> void {
    impl_->SetTitle(title);
}

auto Window::OnResize(ResizeCallback callback) -> void {
    impl_->SetResizeCallback(std::move(callback));
}

Window::~Window() = default;

}