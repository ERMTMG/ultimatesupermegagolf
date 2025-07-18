/*
    FILE: collision_shapes.h
    Defines the basic shapes of which all the colliders in the game are made. Uses an
    inheritance tree with virtual functions, which admittedly may not be the best option,
    but i haven't had problems with it yet (apparently virtual functions are slow but i'm
    not seeing any slowdown). Also defines collision checks for each shape and a general
    """polymorphic""" function that checks if any two shapes are colliding
*/
#pragma once
#include"raylib.h"
#include"utility.h"
#include"basic_components.h"

// Declares an enumerate for each possible type of shape. See further down
enum class CollisionShapeType{
    NONE, POINT, BARRIER, LINE, RECT, CIRCLE 
};
inline const int ENUM_COLLISION_TYPE_SIZE = 6;

/*
    This struct is the base upon which other collision shapes are built. It might be beneficial
    to make it pure virtual since it shpuld probably not be instantiated by itself, but i'm not
    sure how much the game's gonna break if i do so. TODO later: try making this pure virtual
*/
struct CollisionShape{
    Vector2 offset;
    virtual inline CollisionShapeType get_type() const{  return CollisionShapeType::NONE;  };
    CollisionShape(const Vector2& offset): offset(offset) {};
    virtual std::unique_ptr<CollisionShape> clone() const = 0; // allows for deep cloning of shapes
};

// Adds the specified position to the offset of the given shape. ShapeType must inherit from CollisionShape.
template<class ShapeType> ShapeType operator+(const ShapeType& shape, const Position& pos);

// Defines a collision shape consisting of a single point (such as the mouse pointer)
struct CollisionPoint : CollisionShape{
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::POINT;  };
    CollisionPoint(const Vector2& offset): CollisionShape(offset){};
    CollisionPoint(const CollisionPoint& other) : CollisionShape(other.offset) {};
    std::unique_ptr<CollisionShape> clone() const override {
        return std::make_unique<CollisionPoint>(*this);
    }
};

// Defines a collision shape consisting of a half-plane whose border contains offset and 
// with outward-pointing normal forming an angle of normalAngle with the horizontal.
struct CollisionBarrier : CollisionShape{
    float normalAngle;

    CollisionBarrier(const Vector2& offset, float normalAngle): CollisionShape(offset), normalAngle(normalAngle) {};
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::BARRIER;  };
    // Method that just gets a unit normal from the stored angle, since the angle is all that's necessary
    inline Vector2 get_unit_normal() const{
        return {cos(normalAngle), sin(normalAngle)};
    }
    CollisionBarrier(const CollisionBarrier& other) : CollisionBarrier(other.offset, other.normalAngle) {};
    std::unique_ptr<CollisionShape> clone() const override {
        return std::make_unique<CollisionBarrier>(*this);
    }
};

// Defines a collision shape consisting of the line segment between offset and (offset + target)
struct CollisionLine : CollisionShape{
    CollisionLine(const Vector2& offset, const Vector2& target): CollisionShape(offset), target(target) {}
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::LINE;  };
    CollisionLine(const CollisionLine& other) : CollisionLine(other.offset, other.target) {};
    std::unique_ptr<CollisionShape> clone() const override {
        return std::make_unique<CollisionLine>(*this);
    }

    Vector2 target;
};

// Defines a collision shape consisting of the axis-aligned rectangle with top left
// corner at offset and dimensions width * height
struct CollisionRect : CollisionShape{
    CollisionRect(const Vector2& offset, float width, float height): CollisionShape(offset), width(width), height(height) {}
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::RECT;  };
    CollisionRect(const CollisionRect& other) : CollisionRect(other.offset, other.width, other.height) {};
    std::unique_ptr<CollisionShape> clone() const override {
        return std::make_unique<CollisionRect>(*this);
    }

    float width;
    float height;
};

//Defines a collision shape consisting of a disk with center at offset and the given
//radius
struct CollisionCircle : CollisionShape{
    CollisionCircle(const Vector2& offset, float radius): CollisionShape(offset), radius(radius) {};
    inline CollisionShapeType get_type() const override{  return CollisionShapeType::CIRCLE;  };
    CollisionCircle(const CollisionCircle& other) : CollisionCircle(other.offset, other.radius) {};
    std::unique_ptr<CollisionShape> clone() const override {
        return std::make_unique<CollisionCircle>(*this);
    }

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

;// All the functions for collision detection with basic shapes (expandable list). order-sensitive!

CollisionInformation colliding(const CollisionPoint& point, const CollisionBarrier& barrier);
CollisionInformation colliding(const CollisionPoint& point, const CollisionLine& line);
CollisionInformation colliding(const CollisionPoint& point, const CollisionRect& rect);
CollisionInformation colliding(const CollisionPoint& point, const CollisionCircle& circle);
CollisionInformation colliding(const CollisionCircle& circ1, const CollisionCircle& circ2);
CollisionInformation colliding(const CollisionCircle& circle, const CollisionLine& line);
CollisionInformation colliding(const CollisionCircle& circle, const CollisionRect& rect);

/*
    Function that takes in two generic shape pointers and decides which two shapes to use and in which order, to 
    tell if they are colliding when placed in the given positions.
    Returns a CollisionInformation struct whose unit normal always pushes away the *first* argument of the function.
    WARNING: crashes if the two shape types given don't have a collision function integrated!
*/
CollisionInformation process_collision(const CollisionShape* shape1, const CollisionShape* shape2, const Position& pos1 = {0,0}, const Position& pos2 = {0,0});
