//
// Created by usuario on 1/18/25.
//

#ifndef ULTIMATESUPERMEGAGOLF_VECTOR3_UTILITIES_H
#define ULTIMATESUPERMEGAGOLF_VECTOR3_UTILITIES_H
#include "raylib.h"
#include <cmath>
#include <stdexcept>


static const Vector3 VECTOR3_NULL = {NAN, NAN, NAN};

inline Vector3 operator+(const Vector3& v1, const Vector3& v2){
    return Vector3{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

inline Vector3 operator-(const Vector3& v1, const Vector3& v2){
    return Vector3{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

inline Vector3 operator*(const Vector3& v, float scalar){
    return Vector3{v.x * scalar, v.y * scalar, v.z * scalar};
}

inline Vector3 operator*(float scalar, const Vector3& v){
    return v * scalar;
}

inline Vector3 operator/(const Vector3& v, float scalar){
    return Vector3{v.x / scalar, v.y / scalar, v.z / scalar};
}

inline Vector3 operator-(const Vector3& v){
    return v * -1.0;
}

inline float operator*(const Vector3& v1, const Vector3& v2){
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

inline float length(const Vector3& v){
    return std::sqrt(v*v);
}

inline float length_squared(const Vector3& v){
    return v*v;
}

inline Vector3& operator+=(Vector3& v1, const Vector3& v2){
    v1 = v1 + v2;
    return v1;
}

inline Vector3& operator-=(Vector3& v1, const Vector3& v2){
    v1 = v1 - v2;
    return v1;
}

inline Vector3& operator*=(Vector3& v1, float scalar){
    v1 = v1 * scalar;
    return v1;
}

inline Vector3& operator/=(Vector3& v1, float scalar){
    v1 = v1 / scalar;
    return v1;
}

enum RotationAxis{X_AXIS, Y_AXIS, Z_AXIS};
Vector3 rotate(const Vector3& v, float theta, RotationAxis axis = Z_AXIS);






#endif //ULTIMATESUPERMEGAGOLF_VECTOR3_UTILITIES_H
