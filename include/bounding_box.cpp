#include "bounding_box.h"

bool overlapping_bb(const BoundingBox& bb1, const BoundingBox& bb2, const Position& pos1, const Position& pos2){
    Vector2 bb1TruePos = bb1.offset + to_Vector2(pos1);
    Vector2 bb2TruePos = bb2.offset + to_Vector2(pos2);
    return (
        bb1TruePos.x + bb1.width >= bb2TruePos.x &&
        bb1TruePos.x <= bb2TruePos.x + bb2.width &&
        bb1TruePos.y + bb1.height >= bb2TruePos.y &&
        bb1TruePos.y <= bb2TruePos.y + bb2.height
    );
}