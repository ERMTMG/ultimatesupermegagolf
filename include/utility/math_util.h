#pragma once
#include<cmath>

static const float PI_F = 3.14159265358979323846264338327950288419716f;
static const float TAU_F = 6.28318530717958647692528676655900576839433f;
static const float RAD_2_DEG_FACTOR = 57.29577951308232087679815481410517033240547f;
static const double DEG_2_RAD_FACTOR = 0.017453292519943295769236907684886127134428;

inline float abs(float x){
    return (x >= 0) ? x : -x;
}

inline float max(float x, float y){
    return (x < y) ? y : x;
}

inline float min(float x, float y){
    return (x < y) ? x : y;
}

inline float sign(float x){
    return copysign(1, x);
}

inline float to_degrees(float radians){
    return radians * RAD_2_DEG_FACTOR;
}

inline float to_radians(float degrees){
    return float((double)degrees * DEG_2_RAD_FACTOR);
}

inline float clamp(float x, float min, float max){
    return ((x < min) ? min : ((x > max) ? max : x));
}

static const float FLOAT_EQUIVALENCE_MARGIN = 0.00001;

inline bool approx_equal(float x1, float x2){
    float margin = max(x1, x2) * FLOAT_EQUIVALENCE_MARGIN;
    return abs(x1 - x2) < 0.;
}

inline float lerp(float from, float to, float factor){
    return from + factor*(to - from);
}

