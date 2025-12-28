/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_sprite.hpp"

#include <vglx/core.hpp>
#include <vglx/helpers.hpp>

#include <print>

using namespace vglx;

namespace {

auto handle = TextureHandle {};

}

ExampleSprite::ExampleSprite() {
    show_context_menu_ = false;

    Add(Grid::Create({
        .color = 0x333333,
        .size = 4.0f,
        .divisions = 16
    }));
}

auto ExampleSprite::OnAttached(SharedContextPointer context) -> void {
    Add(OrbitControls::Create(context->camera, {
        .radius = 3.0f,
        .pitch = math::pi_over_6
    }));

    handle = context->asset_manager->LoadTexture(
        "assets/sprite/sprite.tex"
    );
}

auto ExampleSprite::OnUpdate(float delta) -> void {
    if (!sprite_) {
        if (auto tex = handle.TryValue()) {
            sprite_ = Add(Sprite::Create(SpriteMaterial::Create(tex.value())));
            sprite_->SetScale(0.15f);
            sprite_->TranslateY(1.0f);
        } else if (auto err = handle.TryError()) {
            std::println(stderr, "{}", err.value());
        }
    }

    if (sprite_) {
        sprite_->rotation += 1.0f * delta;
    }
}
