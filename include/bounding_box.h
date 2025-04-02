/*
    FILE: bounding_box.h
    Defines the bounding box component, used to check if objects are "close enough"
    to each other for it to make sense to check for any interactions between them, or
    if they are even in view of the camera and therefore needed to draw.

*/
#pragma once
#include"raylib.h"
#include"utility.h"
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

// If you're getting this as a result of anything, you're doing something wrong
inline static const BoundingBoxComponent BB_INVALID = {VEC2_NULL, NAN, NAN};

// Checks validity of bounding box so that the above result doesn't fuck you up
inline bool is_bb_valid(const BoundingBoxComponent& bb){
    return (!is_vector2_nan(bb.offset) && bb.width == bb.width && bb.height == bb.height);
}

// Basic bounding box intersection check
bool overlapping_bb(const BoundingBoxComponent& bb1, const BoundingBoxComponent& bb2, const Position& pos1 = {0,0}, const Position& pos2 = {0,0});

// Draws the bounding box to the screen, meant to be used between Raylib's BeginDrawing() and EndDrawing()
void draw_bb_debug(const BoundingBoxComponent& bb, const Position& pos = {0,0});

// Calculates a minimal bounding box that totally covers the given collision shape
BoundingBoxComponent calculate_bb(const CollisionShape* shape);

// Calculates a minimal bounding box that totally covers both given bounding boxes
BoundingBoxComponent bb_union(const BoundingBoxComponent& bb1, const BoundingBoxComponent& bb2);

// Calculates a minimal bounding box that totally covers the given collision component
// with a margin to spare
BoundingBoxComponent calculate_bb(const CollisionComponent& collision, float margin = 0);

// Calculates a minimal bounding box that totally covers the given sprite with a margin
// to spare
BoundingBoxComponent calculate_bb(const SpriteSheet& sprite, float margin = 0);