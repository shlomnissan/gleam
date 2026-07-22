/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "vglx/utilities/stats.hpp"

#include "vglx/utilities/timer.hpp"

#include "utilities/data_series.hpp"

#ifdef VGLX_USE_IMGUI
#include <imgui/imgui.h>
#endif

namespace vglx {

static const float kContainerWidth {180.0f};
static const float kContainerHeight {107.0f};

struct Stats::Impl {
    DataSeries<float, 150> fps_series;
    DataSeries<float, 150> frame_time_series;
    DataSeries<float, 150> rendered_objects_series;

    Timer timer {true};

    double last_flush = 0.0;
    double frame_start = 0.0;
    double frame_time = 0.0;

    unsigned last_objects = 0;
    unsigned frame_count = 0;

    Impl() {
        last_flush = timer.GetElapsedMilliseconds();
    }

    auto Before() {
        const auto now = timer.GetElapsedMilliseconds();

        while (now - last_flush >= 1000.0) {
            fps_series.Push(static_cast<float>(frame_count));
            frame_time_series.Push(static_cast<float>(frame_time));
            rendered_objects_series.Push(last_objects);
            frame_count = 0;
            last_flush += 1000.0;
        }

        frame_start = now;
        ++frame_count;
    }

    auto After(unsigned int n_objects) {
        const auto frame_end = timer.GetElapsedMilliseconds();
        frame_time = frame_end - frame_start;
        last_objects = n_objects;
    }
};

Stats::Stats() : impl_(std::make_unique<Stats::Impl>()) {}

auto Stats::BeforeRender() -> void {
    impl_->Before();
}

auto Stats::AfterRender(unsigned int n_objects) -> void {
    impl_->After(n_objects);
}

auto Stats::Draw() const -> void {
#ifdef VGLX_USE_IMGUI
    const auto window_width = ImGui::GetIO().DisplaySize.x;

    ImGui::SetNextWindowSize({kContainerWidth, kContainerHeight});
    ImGui::SetNextWindowPos({window_width - kContainerWidth - 10.0f, 10.0f});
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("Stats", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoInputs
    );

    ImGui::Text("Frame time: %.0fms", impl_->frame_time_series.LastValue());
    ImGui::Text("Object count: %.0f", impl_->rendered_objects_series.LastValue());
    ImGui::Text("FPS: %.0f", impl_->fps_series.LastValue());

    ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.106f, 0.106f, 0.122f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, {0.149f, 0.682f, 1.0f, 1.0f});
    ImGui::PlotHistogram(
        "##FPS",
        impl_->fps_series.Buffer(), 150, 0, nullptr, 0.0f, 120.0f, {165, 40}
    );
    ImGui::PopStyleColor(2);

    ImGui::End();
#endif
}

Stats::~Stats() = default;

}
