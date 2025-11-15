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
