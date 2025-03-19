#ifndef USMG_COLLISION_SHAPES
#define USMG_COLLISION_SHAPES

#include"collision_shapes.h"
#include<type_traits>
#include"utility.h"
#include<cassert>

template<class ShapeType> ShapeType operator+(const ShapeType& shape, const Position& pos){
    static_assert(std::is_base_of<CollisionShape, ShapeType>::value, "Left operand is not a subclass of CollisionShape");
    ShapeType output = shape;
    output.offset += to_Vector2(pos);
    return output;
}

#endif

std::string to_string(CollisionShapeType type){
    switch (type){
        case CollisionShapeType::NONE: return "None";
        case CollisionShapeType::POINT: return "Point";
        case CollisionShapeType::BARRIER: return "Barrier";
        case CollisionShapeType::LINE: return "Line";
        case CollisionShapeType::RECT: return "Rect";
        case CollisionShapeType::CIRCLE: return "Circle";
        default: return "Unknown";
    }
}

CollisionInformation colliding(const CollisionPoint &point, const CollisionBarrier &barrier){
    CollisionInformation output = {false, barrier.get_unit_normal()};
    float cos_theta = cos(barrier.normalAngle);
    float sin_theta = sin(barrier.normalAngle);
    float diff_x = barrier.offset.x - point.offset.x;
    float diff_y = barrier.offset.y - point.offset.y;

    // The point (x1, y1) collides with the barrier at (x2, y2) with normal angle theta
    // if and only if (x2 - x1)*cos(theta) + (y2 - y1)*sin(theta) >= 0
    // (the derivation of this and other collision conditions is left as an 
    // exercise to the reader)

    if(diff_x * cos_theta + diff_y * sin_theta >= 0){
        output.collision = true;
    }
    return output;
}

CollisionInformation colliding(const CollisionPoint& point, const CollisionLine& line){
    CollisionInformation output;
    const Vector2& v = line.target;
    Vector2 unitNormal = unit_vector({v.y, -v.x});
    output.unitNormal = unitNormal;

    // The point (x, y) collides with the line segment from (x0, y0) to
    // (x0 + xv, y0 + yv) if and only if (x - x0)/xv == (y - y0)/yv
    // and this value is in the interval [0, 1]

    float lhs = (point.offset.x - line.offset.x) / v.x;
    float rhs = (point.offset.y - line.offset.y) / v.y;
    output.collision = (approx_equal(lhs, rhs) && 0.0 <= lhs && lhs <= 1.0);
    return output;
}

CollisionInformation colliding(const CollisionPoint& point, const CollisionRect& rect){
    CollisionInformation output;
    // pretty straightforward collision detection. the harder thing is calculating the normal
    output.collision = (rect.offset.x <= point.offset.x && point.offset.x <= rect.offset.x + rect.width) &&
                       (rect.offset.y <= point.offset.y && point.offset.y <= rect.offset.y + rect.height);
    if(output.collision){
        Vector2 center = rect.offset + 0.5 * Vector2{rect.width, rect.height};
        Vector2 v = point.offset - center; //vector from the center to the point (pushing point off rectangle)
        float lhs = fabs(v.x / rect.width);
        float rhs = fabs(v.y / rect.height);
        if(approx_equal(lhs, rhs)){ // |v_x / w| ~= |v_y / h|, normal vector pushes in both directions
            output.unitNormal = M_SQRT1_2 * Vector2{sign(v.x), sign(v.y)};
        } else if(lhs > rhs){ // |v_x / w| > |v_y / h|, normal pushes in the x direction only
            output.unitNormal = Vector2{sign(v.x), 0};
        } else { // |v_x / w| < |v_y / h|, normal pushes in the y direction only
            output.unitNormal = Vector2{0, sign(v.y)};
        }
    }
    return output;
}

CollisionInformation colliding(const CollisionPoint& point, const CollisionCircle& circle){
    return CollisionInformation {
        (length_squared(point.offset - circle.offset) < circle.radius*circle.radius),
        unit_vector(point.offset - circle.offset)
    };
}

CollisionInformation colliding(const CollisionCircle& circ1, const CollisionCircle& circ2){
    return CollisionInformation {
        (length(circ2.offset - circ1.offset) < (circ1.radius + circ2.radius)),
        unit_vector(circ1.offset - circ2.offset)
    };
}

CollisionInformation colliding(const CollisionCircle& circle, const CollisionLine& line){
    // thank you jeffrey thompson for the algorithm and math

    CollisionInformation output;
    auto[firstEndColliding, normalFromFirstEnd] = colliding(CollisionPoint{line.offset}, circle).reverse_normal();
    auto[secondEndColliding, normalFromSecondEnd] = colliding(CollisionPoint{line.offset + line.target}, circle).reverse_normal();
    if(firstEndColliding || secondEndColliding){
        output.collision = true;
        output.unitNormal = firstEndColliding ? normalFromFirstEnd : normalFromSecondEnd; // if both ends are colliding, then woops
    } else {
        float segmentLengthSquared = length_squared(line.target);
        float dotProduct = ((circle.offset - line.offset) * (line.target)) / segmentLengthSquared;
        if(0 <= dotProduct && dotProduct <= 1){
            Vector2 projectionOnLine = line.offset + dotProduct * line.target;
            output.collision = (length_squared(projectionOnLine - circle.offset) <= circle.radius*circle.radius);
            if(output.collision){
                output.unitNormal = unit_vector(circle.offset - projectionOnLine);
            }
        } else {
            output.collision = false;
        }
    }
    return output;
}

CollisionInformation colliding(const CollisionCircle& circle, const CollisionRect& rect){
    // this one is from some newcastle university document i think

    CollisionInformation output;
    Vector2 rectCenter = rect.offset + Vector2{rect.width / 2, rect.height / 2};
    Vector2 closestPoint = circle.offset - rectCenter;
    closestPoint.x = clamp(closestPoint.x, rect.offset.x, rect.offset.x + rect.width);
    closestPoint.y = clamp(closestPoint.y, rect.offset.y, rect.offset.y + rect.height);
    output.collision = (length_squared(closestPoint - circle.offset) <= circle.radius*circle.radius);
    if(output.collision){
        output.unitNormal = unit_vector(circle.offset - closestPoint);
    }
    return output;
}

CollisionInformation colliding(const CollisionCircle& circle, const CollisionBarrier& barrier){
    static const CollisionInformation noCollision{false, VEC2_ZERO};
    Vector2 n = barrier.get_unit_normal();
    const CollisionInformation collision{true, n};
    if((circle.offset - barrier.offset)*n <= 0){ // circle's center is INSIDE barrier
        return collision;  
    } else {
        float distance = abs(n.x * (circle.offset.x - barrier.offset.x) + n.y * (circle.offset.y - barrier.offset.y)); //distance from circle's center to barrier line
        if(distance <= circle.radius){
          return collision;
        }
    }
    return noCollision;
}

//dont open this (dogshit code)
/*
CollisionInformation colliding(const CollisionShape* shape1, const CollisionShape* shape2, 
                               const Position& pos1 = {0,0}, const Position& pos2 = {0,0}){
    //this is a whole ass hunk of spaghetti, i fear
    const CollisionPoint* shape1Point = dynamic_cast<const CollisionPoint*>(shape1);
    const CollisionBarrier* shape1Barrier = dynamic_cast<const CollisionBarrier*>(shape1);
    const CollisionLine* shape1Line = dynamic_cast<const CollisionLine*>(shape1);
    const CollisionRect* shape1Rect= dynamic_cast<const CollisionRect*>(shape1);
    const CollisionCircle* shape1Circle = dynamic_cast<const CollisionCircle*>(shape1);

    const CollisionPoint* shape2Point = dynamic_cast<const CollisionPoint*>(shape2);
    const CollisionBarrier* shape2Barrier = dynamic_cast<const CollisionBarrier*>(shape2);
    const CollisionLine* shape2Line = dynamic_cast<const CollisionLine*>(shape2);
    const CollisionRect* shape2Rect= dynamic_cast<const CollisionRect*>(shape2);
    const CollisionCircle* shape2Circle = dynamic_cast<const CollisionCircle*>(shape2);

    if(shape1Circle && shape2Circle) return colliding_circle_circle(*shape1Circle + pos1, *shape2Circle + pos2);
    if(shape1Circle && shape2Rect) return colliding_circle_rect(*shape1Circle + pos1, *shape2Rect + pos2);
    if(shape1Rect && shape2Circle){ auto output = colliding_circle_rect(*shape2Circle + pos2, *shape1Rect + pos1); output.unitNormal = -output.unitNormal; return output; }
    if(shape1Circle && shape2Line) return colliding_circle_line(*shape1Circle + pos1, *shape2Line + pos2);
    if(shape1Line && shape2Circle){ auto output = colliding_circle_line(*shape2Circle + pos2, *shape1Line + pos1); output.unitNormal = -output.unitNormal; return output; }
    if(shape1Circle && shape2Point){ auto output = colliding_point_circle(*shape2Point + pos2, *shape1Circle + pos1); output.unitNormal = -output.unitNormal; return output; }
    if(shape1Point && shape2Circle) return colliding_point_circle(*shape1Point + pos1, *shape2Circle + pos2);
    if(shape1Point && shape2Barrier) return colliding_point_barrier(*shape1Point + pos1, *shape2Barrier + pos2);
    if(shape1Barrier && shape2Point){ auto output = colliding_point_barrier(*shape2Point + pos2, *shape1Barrier + pos1); output.unitNormal = -output.unitNormal; return output; }
    if(shape1Point && shape2Rect) return colliding_point_rect(*shape1Point + pos1, *shape2Rect + pos2);
    if(shape1Rect && shape2Point){ auto output = colliding_point_rect(*shape2Point + pos2, *shape1Rect + pos1); output.unitNormal = -output.unitNormal; return output; }
    if(shape1Point && shape2Line) return colliding_point_line(*shape1Point + pos1, *shape2Line + pos2);
    if(shape1Line && shape2Point){ auto output = colliding_point_line(*shape2Point + pos2, *shape1Line + pos1); output.unitNormal = -output.unitNormal; return output; }
    else{
        throw std::invalid_argument("Interaction between collision shapes " + to_string(shape1->get_type()) + " and " + to_string(shape2->get_type()) + " not supported");
    }
}
*/

constexpr inline int key(CollisionShapeType type1, CollisionShapeType type2){
    return (int)type1 * ENUM_COLLISION_TYPE_SIZE + (int)type2;
}


CollisionInformation process_collision(const CollisionShape* shape1, const CollisionShape* shape2, const Position& pos1, const Position& pos2){
    assert(shape1 != nullptr && shape2 != nullptr);
    CollisionShapeType type1 = shape1->get_type();
    CollisionShapeType type2 = shape2->get_type();
    int currentKey = key(type1, type2);
    switch (currentKey){
      case key(CollisionShapeType::CIRCLE, CollisionShapeType::CIRCLE): {
        const CollisionCircle* circle1 = static_cast<const CollisionCircle*>(shape1);
        const CollisionCircle* circle2 = static_cast<const CollisionCircle*>(shape2);
        return colliding(*circle1 + pos1, *circle2 + pos2);
      }
      case key(CollisionShapeType::CIRCLE, CollisionShapeType::RECT):{
        const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape1);
        const CollisionRect* rect = static_cast<const CollisionRect*>(shape2);
        return colliding(*circle + pos1, *rect + pos2);
      }
      case key(CollisionShapeType::RECT, CollisionShapeType::CIRCLE):{
        const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape2);
        const CollisionRect* rect = static_cast<const CollisionRect*>(shape1);
        return colliding(*circle + pos2, *rect + pos1).reverse_normal();
      }
      case key(CollisionShapeType::CIRCLE, CollisionShapeType::LINE):{
        const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape1);
        const CollisionLine* line = static_cast<const CollisionLine*>(shape2);
        return colliding(*circle + pos1, *line + pos2);
      }
      case key(CollisionShapeType::LINE, CollisionShapeType::CIRCLE):{
        const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape2);
        const CollisionLine* line = static_cast<const CollisionLine*>(shape1);
        return colliding(*circle + pos2, *line + pos1).reverse_normal();
      }
      case key(CollisionShapeType::CIRCLE, CollisionShapeType::POINT):{
        const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape1);
        const CollisionPoint* point = static_cast<const CollisionPoint*>(shape2);
        return colliding(*point + pos2, *circle + pos1).reverse_normal();
      }
      case key(CollisionShapeType::POINT, CollisionShapeType::CIRCLE):{
        const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape2);
        const CollisionPoint* point = static_cast<const CollisionPoint*>(shape1);
        return colliding(*point + pos1, *circle + pos2);
      }
      case key(CollisionShapeType::POINT, CollisionShapeType::RECT):{
        const CollisionPoint* point = static_cast<const CollisionPoint*>(shape1);
        const CollisionRect* rect = static_cast<const CollisionRect*>(shape2);
        return colliding(*point + pos1, *rect + pos2);
      }
      case key(CollisionShapeType::RECT, CollisionShapeType::POINT):{
        const CollisionPoint* point = static_cast<const CollisionPoint*>(shape2);
        const CollisionRect* rect = static_cast<const CollisionRect*>(shape1);
        return colliding(*point + pos2, *rect + pos1).reverse_normal();
      }
      case key(CollisionShapeType::POINT, CollisionShapeType::BARRIER):{
        const CollisionPoint* point = static_cast<const CollisionPoint*>(shape1);
        const CollisionBarrier* barrier = static_cast<const CollisionBarrier*>(shape2);
        return colliding(*point + pos1, *barrier + pos2);
      }
      case key(CollisionShapeType::BARRIER, CollisionShapeType::POINT):{
        const CollisionPoint* point = static_cast<const CollisionPoint*>(shape2);
        const CollisionBarrier* barrier = static_cast<const CollisionBarrier*>(shape1);
        return colliding(*point + pos2, *barrier + pos1).reverse_normal();
      }
      case key(CollisionShapeType::POINT, CollisionShapeType::LINE):{
        const CollisionPoint* point = static_cast<const CollisionPoint*>(shape1);
        const CollisionLine* line = static_cast<const CollisionLine*>(shape2);
        return colliding(*point + pos1, *line + pos2);
      }
      case key(CollisionShapeType::LINE, CollisionShapeType::POINT):{
        const CollisionPoint* point = static_cast<const CollisionPoint*>(shape2);
        const CollisionLine* line = static_cast<const CollisionLine*>(shape1);
        return colliding(*point + pos2, *line + pos1).reverse_normal();
      }
      case key(CollisionShapeType::CIRCLE, CollisionShapeType::BARRIER):{
        const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape1);
        const CollisionBarrier* barrier = static_cast<const CollisionBarrier*>(shape2);
        return colliding(*circle + pos1, *barrier + pos2);
      }
      case key(CollisionShapeType::BARRIER, CollisionShapeType::CIRCLE):{
        const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape2);
        const CollisionBarrier* barrier = static_cast<const CollisionBarrier*>(shape1);
        return colliding(*circle + pos1, *barrier + pos2).reverse_normal();
      }
      default: throw std::invalid_argument("Interaction between shapes " + to_string(type1) + " and " + to_string(type2) + " not supported");
    }
}

