#pragma once
#include"raylib.h"
#include<cmath>
#include<iostream>

//#define max(x,y) ((x) < (y) ? (y) : (x))
//#define sign(x) ((float)copysign(1, (x)))
//#define clamp(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
inline float max(float x, float y){
    return (x < y) ? y : x;
}

inline float min(float x, float y){
    return (x < y) ? x : y;
}

inline float sign(float x){
    return copysign(1, x);
}

inline float clamp(float x, float min, float max){
    return ((x < min) ? min : ((x > max) ? max : x));
}

static const Vector2 VEC2_NULL = {NAN, NAN};
static const Vector2 VEC2_ZERO = {0,0};
static const Vector2 VEC2_ONE = {1,1};
static const Vector2 VEC2_UP_UNIT = {0,-1};
static const Vector2 VEC2_DOWN_UNIT = {0,1};
static const Vector2 VEC2_LEFT_UNIT = {-1,0};
static const Vector2 VEC2_RIGHT_UNIT = {1,0};

inline Vector2 operator+(const Vector2& v1, const Vector2& v2){
    return {v1.x + v2.x, v1.y + v2.y};
}

inline Vector2 operator-(const Vector2& v1, const Vector2& v2){
    return {v1.x - v2.x, v1.y - v2.y};
}

inline Vector2 operator*(const Vector2& v, float scalar){
    return {v.x * scalar, v.y * scalar};
}

inline Vector2 operator*(float scalar, const Vector2& v){
    return v * scalar;
}

inline Vector2 operator/(const Vector2& v, float scalar){
    return {v.x / scalar, v.y / scalar};
}

inline Vector2 operator-(const Vector2& v){
    return v * -1.0;
}

inline float operator*(const Vector2& v1, const Vector2& v2){
    return (v1.x * v2.x) + (v1.y * v2.y);
}

inline bool operator==(const Vector2& v1, const Vector2& v2){
    return (v1.x == v2.x && v1.y == v2.y);
}

inline bool operator!=(const Vector2& v1, const Vector2& v2){
    return !(v1 == v2);
}

inline float length(const Vector2& v){
    return std::sqrt(v*v);
}

inline float length_squared(const Vector2& v){
    return v*v;
}

inline Vector2& operator+=(Vector2& v1, const Vector2& v2){
    v1 = v1 + v2;
    return v1;
}

inline Vector2& operator-=(Vector2& v1, const Vector2& v2){
    v1 = v1 - v2;
    return v1;
}

inline Vector2& operator*=(Vector2& v1, float scalar){
    v1 = v1 * scalar;
    return v1;
}

inline Vector2& operator/=(Vector2& v1, float scalar){
    v1 = v1 / scalar;
    return v1;
}

inline Vector2 unit_vector(const Vector2& v){
    return v / length(v);
}

inline float angle(const Vector2& v1, const Vector2& v2){
    return acos( (v1 * v2) / (length(v1)*length(v2)) );
}

inline Vector2 reflect_across_normal(const Vector2& v, const Vector2& unitNormal){
    return v - 2 * (unitNormal * v) * unitNormal;
}

inline bool approx_equal(float x1, float x2){
    float margin = max(x1, x2) / 100000;
    return fabs(x1 - x2) < 0.;
}

inline std::ostream& operator<<(std::ostream& out, const Vector2& v){
    out << '(' << v.x << ',' << v.y << ')';
    return out;
}
