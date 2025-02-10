#include "vector3_utilities.h"

Vector3 rotate(const Vector3& v, float theta, RotationAxis axis){
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