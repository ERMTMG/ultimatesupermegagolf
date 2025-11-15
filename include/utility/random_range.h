#pragma once
#include "../raylib.h"
#include"../rng_component.h"
#include "color_util.h"

#include<cassert>

struct IntRange {
    int min;
    int max;
    IntRange(int min, int max) : min(min), max(max) {
        assert(min <= max);
    }
    int get_random(RNGComponent& rng) const {
        unsigned int diff = max - min + 1;
        return random_int(rng, diff) + min;
    }
    int get_random_fast(RNGComponent& rng) const {
        unsigned int diff = max - min + 1;
        return random_int_fast(rng, diff) + min;
    }
};

struct FloatRange {
    float min;
    float max;
    FloatRange(float min, float max) : min(min), max(max) {
        assert(min <= max);
    }
    float get_random(RNGComponent& rng) const {
        return random_float(rng, min, max);
    }
};

struct Vec2Range {
    Vector2 min;
    Vector2 max;
    Vec2Range(const Vector2& min, const Vector2& max) : min(min), max(max) {
        assert(min.x < max.x && min.y < max.y);
    }
    Vector2 get_random(RNGComponent& rng) const {
        return Vector2{random_float(rng, min.x, max.x), random_float(rng, min.y, max.y)};
    }
};

struct ColorRange {
    Color from;
    Color to;
    ColorRange(const Color& from, const Color& to) : from(from), to(to) {};
    Color get_random(RNGComponent& rng) const {
        return lerp(from, to, random_float(rng));
    }
};
