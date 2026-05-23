/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "example_sprite.hpp"

#include <print>

using namespace vglx;

ExampleSprite::ExampleSprite(Camera* camera) {
    show_context_menu_ = false;

    Add(OrbitControls::Create(camera, {
        .radius = 1.0f,
        .pitch = math::pi_over_6
    }));

    Add(Grid::Create({
        .color = 0x333333,
        .size = 4.0f,
        .divisions = 16
    }))->TranslateY(-1.0f);

    auto texture = LoadTexture(ASSETS_DIR "/sprite/sprite.png");
    if (texture.has_value()) {
        sprite_ = Add(Sprite::Create(SpriteMaterial::Create(texture.value())));
        sprite_->SetScale(0.3f);
    } else {
        std::println(stderr, "{}", texture.error());
    }
}

auto ExampleSprite::OnUpdate(float delta) -> void {
    if (sprite_) {
        sprite_->rotation += 1.0f * delta;
    }
}
