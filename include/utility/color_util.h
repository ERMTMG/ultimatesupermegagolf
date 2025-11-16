#pragma once
#include "../raylib.h"
#include <cstdint>

inline Color lerp(const Color& from, const Color& to, float factor){
    return Color {
        .r = from.r + uint8_t(factor * to.r),
        .g = from.g + uint8_t(factor * to.g),
        .b = from.b + uint8_t(factor * to.b),
        .a = from.a + uint8_t(factor * to.a)
    };
}

Color& color_assign(Color& c1, Color c2){
    c1.r = c2.r;
    c1.g = c2.g;
    c1.b = c2.b;
    c1.a = c2.a;
    return c1;
}
