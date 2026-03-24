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

auto handle = TextureLoadHandle {};

}

ExampleSprite::ExampleSprite() {
    show_context_menu_ = false;

    Add(Grid::Create({
        .color = 0x333333,
        .size = 4.0f,
        .divisions = 16
    }))->TranslateY(-1.0f);

}

auto ExampleSprite::OnAttached(SharedContextPointer context) -> void {
    Add(OrbitControls::Create(context->camera, {
        .radius = 1.0f,
        .pitch = math::pi_over_6
    }));

    handle = context->texture_loader->LoadAsync (
        ASSETS_DIR "/sprite/sprite.png"
    );
}

auto ExampleSprite::OnUpdate(float delta) -> void {
    if (auto tex = handle.TryTake()) {
        sprite_ = Add(Sprite::Create(SpriteMaterial::Create(tex.value())));
        sprite_->SetScale(0.3f);
    }

    if (sprite_) {
        sprite_->rotation += 1.0f * delta;
    }
}
