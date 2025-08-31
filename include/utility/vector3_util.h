//
// Created by usuario on 1/18/25.
//

#pragma once
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

inline Vector3 rotate(const Vector3& v, float theta, RotationAxis axis){
    Vector3 output;
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);
    float firstCoordinate; float secondCoordinate;
    switch(axis){
      case Z_AXIS:
        firstCoordinate = v.x;
        secondCoordinate = v.y;
        break;
      case Y_AXIS:
        firstCoordinate = v.x;
        secondCoordinate = v.z;
        break;
      case X_AXIS:
        firstCoordinate = v.y;
        secondCoordinate = v.z;
        break;
      default:
        throw std::invalid_argument("Axis around which to rotate point is neither of \"X_AXIS\", \"Y_AXIS\" or \"Z_AXIS\"");
    }
    //Regardless of which axis you rotate around, the result of the rotated point is
    //a' = a*cos(theta) + b*sin(theta);  b' = b*cos(theta) - a*sin(theta)
    //where a and b are the two coordinates distinct from the axis of rotation. The other coordinate stays the same
    float rotatedFirstCoordinate = firstCoordinate*cosTheta + secondCoordinate*sinTheta;
    float rotatedSecondCoordinate = secondCoordinate*cosTheta - firstCoordinate*sinTheta;
    switch(axis){
      case Z_AXIS:
        return{rotatedFirstCoordinate, rotatedSecondCoordinate, v.z};
      case Y_AXIS:
        return{rotatedFirstCoordinate, v.y, rotatedSecondCoordinate};
      case X_AXIS:
        return{v.x, rotatedFirstCoordinate, rotatedSecondCoordinate};
    }
    return {};
}
