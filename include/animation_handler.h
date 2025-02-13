#pragma once
#include"raylib.h"
#include"basic_components.h"

struct AnimationHandler{
    void (*handler)(float, float&, SpriteSheet&);
    float timer = 0;
};

template<int FRAME_DURATION_MILLISECONDS>
void default_animation_handler_fn(float delta, float& timer, SpriteSheet& sprite);

template<int FRAME_DURATION_MILLISECONDS = 166>
AnimationHandler default_animation_handler();