#pragma once
#include"raylib.h"
#include"vector2_utilities.h"
#include"basic_components.h"
#include"collision_component.h"

/*
Basic bounding box component. All entities with a drawable component or a 
collision component should have a bounding box. It is not recommended for
a drawable entity to not have a bounding box as it will always be drawnin
that case, and a collidable entity must always have a bounding box for it
to be able to collide with any other entity.
*/
struct BoundingBoxComponent{
    /* Offset with respect to the entity's position. Should almost always have negative components. */
    Vector2 offset;
    float width;
    float height;
};

inline static const BoundingBoxComponent BB_INVALID = {VEC2_NULL, NAN, NAN};

inline bool is_bb_valid(const BoundingBoxComponent& bb){
    return (bb.offset != VEC2_NULL && bb.width == bb.width && bb.height == bb.height);
}

bool overlapping_bb(const BoundingBoxComponent& bb1, const BoundingBoxComponent& bb2, const Position& pos1 = {0,0}, const Position& pos2 = {0,0});

void draw_bb_debug(const BoundingBoxComponent& bb, const Position& pos = {0,0});

BoundingBoxComponent calculate_bb(const CollisionShape* shape);

BoundingBoxComponent bb_union(const BoundingBoxComponent& bb1, const BoundingBoxComponent& bb2);

BoundingBoxComponent calculate_bb(const CollisionComponent& collision, float margin = 0);

BoundingBoxComponent calculate_bb(const SpriteSheet& sprite, float margin = 0);