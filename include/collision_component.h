#pragma once
#include"raylib.h"
#include"entt.hpp"
#include"utility.h"
#include"basic_components.h"
#include"collision_shapes.h"
#include<vector>
#include<memory>

typedef unsigned char LayerType;

struct CollisionComponent{
    std::vector<std::unique_ptr<CollisionShape>> shapes;  //   24 bytes
    unsigned short layerFlags;                            //   +2 bytes
    bool isStatic;                                        //   +1 byte = 27 bytes ==> 5 bytes left to keep 32 byte size
        #define COLLISION_COMPONENT_STATIC true 
        #define COLLISION_COMPONENT_NOT_STATIC false
    
    CollisionComponent(unsigned short layer = 0, bool isStatic = true);
    CollisionComponent(CollisionShape* shape, unsigned short layer = 0, bool isStatic = true);
    CollisionComponent(const CollisionComponent&) = delete;
    CollisionComponent& operator=(const CollisionComponent&) = delete;
    void add_circle(float radius, const Vector2& pos = {0,0});
    void add_rect(float width, float height, const Vector2& pos = {0,0});
    void add_rect_centered(float width, float height);
    void add_line(const Vector2& pos1, const Vector2& pos2);
    void add_barrier(const Vector2& pos, const Vector2& direction);
    void add_point(const Vector2& point);
};

inline constexpr unsigned NUMBER_OF_LAYERS = sizeof(CollisionComponent::layerFlags) * 8;

void set_layers(CollisionComponent& collision, std::vector<LayerType>&& layers);

inline void add_to_layer(CollisionComponent& collision, LayerType layer){
    if(layer >= NUMBER_OF_LAYERS) throw std::invalid_argument("layer must be less than " + std::to_string(NUMBER_OF_LAYERS));
    collision.layerFlags |= (1 << layer);
}

inline void remove_from_layer(CollisionComponent& collision, LayerType layer){
    if(layer >= NUMBER_OF_LAYERS) throw std::invalid_argument("layer must be less than " + std::to_string(NUMBER_OF_LAYERS));
    collision.layerFlags &= ~(1 << layer);
}

inline bool has_common_layers(const CollisionComponent& collision1, const CollisionComponent& collision2){
    return ((collision1.layerFlags & collision2.layerFlags) > 0);
}

/*
Returns the total collision information of the collision between the given components and the given positions.
For the result, it checks each shape of collision1 against each shape of collision2, averaging the normals of all
the collisions for each shape and then averaging the normals of each shape for the total normal vector outputted. 
*/
CollisionInformation get_collision(const CollisionComponent& collision1, const CollisionComponent& collision2, const Position& pos1 = {0,0}, const Position& pos2 = {0,0});

/*
Modifies the moving object's position such that its collider is no longer colliding with staticCollision. Assumes the 
"static" object doesn't move and that the given CollisionInformation indicates the two objects are colliding
*/
void move_object_out_of_collision(const CollisionComponent& movingCollision, const CollisionComponent& staticCollision, Position& movingPosition, const Position& staticPosition, const CollisionInformation& info);

/*
Similar to last function, except now both objects are not static and move out of each other's collisions. Object with 
collision1 moves in the same direction as info.unitNormal and object with collision2 moves in the opposite direction as
info.unitNormal
*/
void mutually_move_objects_out_of_collision(const CollisionComponent& collision1, const CollisionComponent& collision2, Position& pos1, Position& pos2, const CollisionInformation& info);

void draw_collision_debug(const CollisionComponent& collision, const Position& pos = {0,0});

/* 
Component that just stores the entityID of the corresponding entity.
If no collisions are detected, the entityID is set to entt::null.
*/
struct CollisionEntityStoreComponent{
    entt::entity collidedEntityID;
};