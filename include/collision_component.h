/*
    FILE: collision_component.h
    Defines the actual collision component that game objects have, 
    with the collision_shapes.h file being a more internal module that only 
    defines the hitbox shapes.
*/
#pragma once
#include"raylib.h"
#include"entt.hpp"
#include"utility.h"
#include"basic_components.h"
#include"collision_shapes.h"
#include<vector>
#include<memory>

typedef unsigned char LayerType;

/*
    Component that defines the full hitbox of an object as well as the
    rest of the relevant collision information: the collision layers, whether
    or not the object is static,... 
    More information defined on the member functions.
*/
struct CollisionComponent{
    /*
        Defines the actual hitbox of the object. Most objects won't really have more
        than five collision shapes, so using a vector here is probably overkill - BUT
        it might be useful if the whole collision of a level is stored in only one
        CollisionComponent
    */
    std::vector<std::unique_ptr<CollisionShape>> shapes;  

    // Bitwise flag int that defines up to 16 layers the object can belong to. Two
    // objects may only collide if they have at least one layer in common.
    unsigned short layerFlags;                            

    // Static objects don't move, don't collide with other static objects and don't
    // react when another dynamic object collides with them.
    bool isStatic;                                        
        #define COLLISION_COMPONENT_STATIC true 
        #define COLLISION_COMPONENT_NOT_STATIC false
    
    // SIZE CALCULATIONS:
    // 24 bytes (vector) + 2 bytes (layer flags) + 1 byte (isStatic) = 27 bytes -> 5 bytes left until size is > 32

    // defaulted moves
    CollisionComponent(CollisionComponent&&) noexcept            = default;
    CollisionComponent& operator=(CollisionComponent&&) noexcept = default;

    // disable copying
    CollisionComponent(const CollisionComponent&)            = delete;
    CollisionComponent& operator=(const CollisionComponent&) = delete;
    
    // Normal constructor. NOTE: this and other constructors reserve a capacity of three for the shape vector
    CollisionComponent(unsigned short layer = 0, bool isStatic = true);
    // Constructs a component from a shape, the hitbox being only that shape.
    // Intended syntax: CollisionComponent collision(new CollisionCircle(VEC2_ZERO, 16), 1, true);
    CollisionComponent(CollisionShape* shape, unsigned short layer = 0, bool isStatic = true);
    ;// No copy constructor/assignment? TODO later: figure out why not
    //CollisionComponent(const CollisionComponent&) = delete;
    //CollisionComponent& operator=(const CollisionComponent&) = delete;
    ;//Basic methods that just add shapes to the collision component. add_rect_centered is special 
    ;//in that it, well, centers the rectangle at {0,0}.
    void add_circle(float radius, const Vector2& pos = {0,0});
    void add_rect(float width, float height, const Vector2& pos = {0,0});
    void add_rect_centered(float width, float height);
    void add_line(const Vector2& pos1, const Vector2& pos2);
    void add_barrier(const Vector2& pos, const Vector2& direction);
    void add_point(const Vector2& point);
};

// of course, if layerFlags is only 16 bits then we can only have 16 layers
inline constexpr unsigned NUMBER_OF_LAYERS = sizeof(CollisionComponent::layerFlags) * 8;

// Clears and sets the layers of a collision component from a vector of numbers corresponding to layers.
// Intended syntax: set_layers(collision, {1,2,5,8});
void set_layers(CollisionComponent& collision, std::vector<LayerType>&& layers);

// Adds a collision component to a layer without resetting the rest.
inline void add_to_layer(CollisionComponent& collision, LayerType layer){
    if(layer >= NUMBER_OF_LAYERS) throw std::invalid_argument("layer must be less than " + std::to_string(NUMBER_OF_LAYERS));
    collision.layerFlags |= (1 << layer);
}

// Removes a collision component from a layer without resetting the rest
inline void remove_from_layer(CollisionComponent& collision, LayerType layer){
    if(layer >= NUMBER_OF_LAYERS) throw std::invalid_argument("layer must be less than " + std::to_string(NUMBER_OF_LAYERS));
    collision.layerFlags &= ~(1 << layer);
}

// checks if the collision components have any common layers, i.e. if they are eligible to collide with each other
inline bool has_common_layers(const CollisionComponent& collision1, const CollisionComponent& collision2){
    return ((collision1.layerFlags & collision2.layerFlags) > 0);
}

// Clones the collision component source into destination. Assumes that source and destination aren't the same object and that destination is "empty".
void clone_collision(const CollisionComponent& source, CollisionComponent& destination);

// Adds all shapes of collision2 to collision1 with an added offset given by pos. Doesn't modify the layers of collision1, just adds the shapes.
void add_collision(CollisionComponent& collision1, const CollisionComponent& collision2, const Position& pos = {0,0});

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

// Draws the collision shapes of the given component to the screen. Again, meant to be used between BeginDrawing() and EndDrawing()
void draw_collision_debug(const CollisionComponent& collision, const Position& pos = {0,0});

/* 
Component that just stores the entityID of the corresponding entity.
If no collisions are detected, the entityID is set to entt::null.
*/
struct CollisionEntityStoreComponent{
    entt::entity collidedEntityID;
};