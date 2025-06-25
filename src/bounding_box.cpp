#include "bounding_box.h"

bool overlapping_bb(const BoundingBoxComponent& bb1, const BoundingBoxComponent& bb2, const Position& pos1, const Position& pos2){
    Vector2 bb1TruePos = bb1.offset + to_Vector2(pos1);
    Vector2 bb2TruePos = bb2.offset + to_Vector2(pos2);
    return (
        bb1TruePos.x + bb1.width >= bb2TruePos.x &&
        bb1TruePos.x <= bb2TruePos.x + bb2.width &&
        bb1TruePos.y + bb1.height >= bb2TruePos.y &&
        bb1TruePos.y <= bb2TruePos.y + bb2.height
    );
}

void draw_bb_debug(const BoundingBoxComponent& bb, const Position& pos){
    static const Color DEBUG_BOUNDING_BOX_COLOR = Color{0,255,255,255};

    Vector2 bbTruePos = bb.offset + to_Vector2(pos);
    DrawRectangleLines(bbTruePos.x, bbTruePos.y, bb.width, bb.height, DEBUG_BOUNDING_BOX_COLOR);
}

//TODO: something in these calculate_bb functions is awry. fix whatever it is, maybe do another separate program

BoundingBoxComponent calculate_bb(const CollisionShape* shape){
    if(shape == nullptr){
        return BB_INVALID;
    } else {
        // NOTE: this is bad practice. ideally, this should be delegated to a virtual method
        // in the CollisionShape struct, but that would require the file collision_shapes.h to
        // know about bounding_box.h, and that file includes functions with collision components
        // and yadda yadda yadda, plus doing another virtual method might add runtime costs (idk
        // for sure) so im sticking with a switch statement. the fuck you gonna do about it, cry?
        // maybe piss your pants?
        switch (shape->get_type()){
          case CollisionShapeType::CIRCLE: {
            const CollisionCircle* circle = static_cast<const CollisionCircle*>(shape);
            Vector2 bbOffset = circle->offset - Vector2{circle->radius, circle->radius};
            return BoundingBoxComponent{bbOffset, 2*circle->radius, 2*circle->radius};
          }
          case CollisionShapeType::RECT: {
            const CollisionRect* rect = static_cast<const CollisionRect*>(shape);
            return BoundingBoxComponent{rect->offset, rect->width, rect->height};
          }
          case CollisionShapeType::LINE: {
            const CollisionLine* line = static_cast<const CollisionLine*>(shape);
            Vector2 lineEnd = line->offset + line->target;
            Vector2 minEdge = {min(line->offset.x, lineEnd.x), min(line->offset.y, lineEnd.y)};
            Vector2 maxEdge = {max(line->offset.x, lineEnd.x), max(line->offset.y, lineEnd.y)};
            return BoundingBoxComponent{minEdge, (maxEdge - minEdge).x, (maxEdge - minEdge).y};
          }
          case CollisionShapeType::POINT: {
            return BoundingBoxComponent{shape->offset, 0, 0};
          }
          default:
            return BB_INVALID;
        }
    }
}

BoundingBoxComponent bb_union(const BoundingBoxComponent& bb1, const BoundingBoxComponent& bb2){
    if(is_bb_valid(bb1) && is_bb_valid(bb2)){
        Vector2 min1 = bb1.offset; 
        Vector2 min2 = bb2.offset;
        Vector2 max1 = bb1.offset + Vector2{bb1.width, bb1.height};
        Vector2 max2 = bb2.offset + Vector2{bb2.width, bb2.height};
        Vector2 overallMin = {min(min1.x, min2.x), min(min1.y, min2.y)};
        Vector2 overallMax = {max(max1.x, max2.x), max(max2.y, max2.y)};
        return BoundingBoxComponent{overallMin, (overallMax - overallMin).x, (overallMax - overallMin).y};
    } else return BB_INVALID;
}

BoundingBoxComponent calculate_bb(const CollisionComponent& collision, float margin){
    const auto& shapeVec = collision.shapes;
    BoundingBoxComponent output;
    if(shapeVec.empty()){
        return BB_ZERO;
    } else {
        const CollisionShape* firstShape = shapeVec[0].get();
        output = calculate_bb(firstShape);
        for(size_t i = 1; i < shapeVec.size(); i++){
            const CollisionShape* ithShape = shapeVec[i].get();
            BoundingBoxComponent ithBB = calculate_bb(ithShape);
            output = bb_union(output, ithBB);
            if(!is_bb_valid(output)){
                return output; // no matter what we do it's still gonna be invalid so might as well return it directly
            }
        }

    }
    if(margin != 0){
        output.offset -= {margin, margin};
        output.width += margin; output.height += margin;
    }
    return output;
}

BoundingBoxComponent calculate_bb(const SpriteSheet& sprite, float margin){
    int frameWidth = sprite.texture.width / sprite.numberFramesPerRow;
    int frameHeight = sprite.texture.height / sprite.numberRows;
    BoundingBoxComponent output {{-frameWidth/2, -frameHeight/2}, 0, 0};
    output.width = frameWidth;
    output.height = frameHeight;
    if(margin != 0){
        output.offset -= {margin, margin};
        output.width += margin; output.height += margin;
    }
    return output;
}
