#pragma once
#include"raylib.h"
#include"vector2_utilities.h"
#include"basic_components.h"

enum class CollisionShapeType{
    NONE, POINT, BARRIER, LINE, RECT, CIRCLE 
};
inline const int ENUM_COLLISION_TYPE_SIZE = 6;
struct CollisionShape{ //Not meant to be instantiated - abstract struct sort of
    Vector2 offset;
    virtual inline CollisionShapeType get_type() const{  return CollisionShapeType::NONE;  };
    CollisionShape(const Vector2& offset): offset(offset) {};
};

//Adds the specified position to the offset of the given shape. Obviously ShapeType must inherit from CollisionShape.
template<class ShapeType> ShapeType operator+(const ShapeType& shape, const Position& pos);

//Defines a collision shape consisting of a single point (such as the mouse pointer)
struct CollisionPoint : CollisionShape{
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::POINT;  };
    CollisionPoint(const Vector2& offset): CollisionShape(offset){};
};

//Defines a collision shape consisting of a half-plane whose border contains offset and 
//with outward-pointing normal forming an angle of normalAngle
struct CollisionBarrier : CollisionShape{
    float normalAngle;

    CollisionBarrier(const Vector2& offset, float normalAngle): CollisionShape(offset), normalAngle(normalAngle) {};
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::BARRIER;  };
    inline Vector2 get_unit_normal() const{
        return {cos(normalAngle), sin(normalAngle)};
    }
};

//Defines a collision shape consisting of the line segment between offset and (offset + target)
struct CollisionLine : CollisionShape{
    CollisionLine(const Vector2& offset, const Vector2& target): CollisionShape(offset), target(target) {}
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::LINE;  };

    Vector2 target;
};

//Defines a collision shape consisting of the axis-aligned rectangle with top left
//corner at offset and dimensions width x height
struct CollisionRect : CollisionShape{
    CollisionRect(const Vector2& offset, float width, float height): CollisionShape(offset), width(width), height(height) {}
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::RECT;  };

    float width;
    float height;
};

//Defines a collision shape consisting of a disk with center at offset and the given
//radius
struct CollisionCircle : CollisionShape{
    CollisionCircle(const Vector2& offset, float radius): CollisionShape(offset), radius(radius) {};
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::CIRCLE;  };

    float radius;
};

std::string to_string(CollisionShapeType type);

//Return type of collision detection functions - includes a bool indicating whether
//there is a collision and a unit normal vector of the collision that pushes away 
//the **first argument** of the collision function.
struct CollisionInformation{
    bool collision;
    Vector2 unitNormal;
    inline CollisionInformation reverse_normal() const{
        CollisionInformation output = *this;
        output.unitNormal = -unitNormal;
        return output;
    }
};


CollisionInformation colliding(const CollisionPoint& point, const CollisionBarrier& barrier);
CollisionInformation colliding(const CollisionPoint& point, const CollisionLine& line);
CollisionInformation colliding(const CollisionPoint& point, const CollisionRect& rect);
CollisionInformation colliding(const CollisionPoint& point, const CollisionCircle& circle);
CollisionInformation colliding(const CollisionCircle& circ1, const CollisionCircle& circ2);
CollisionInformation colliding(const CollisionCircle& circle, const CollisionLine& line);
CollisionInformation colliding(const CollisionCircle& circle, const CollisionRect& rect);

//CollisionInformation colliding(const CollisionShape* shape1, const CollisionShape* shape2, const Position& pos1 = {0,0}, const Position& pos2 = {0,0});

CollisionInformation process_collision(const CollisionShape* shape1, const CollisionShape* shape2, const Position& pos1 = {0,0}, const Position& pos2 = {0,0});
