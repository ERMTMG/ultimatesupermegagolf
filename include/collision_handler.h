/*
    FILE: collision_handler.h
    Defines the CollisionHandler component, which allows entities to react to collision in
    distinct ways, as well as a default collision handler that implements an elastic collision
    (bouncing-off).
*/
#pragma once
#include"entt.hpp"
#include"raylib.h"
#include"utility.h"
#include"basic_components.h"
#include"collision_shapes.h"
#include"collision_component.h"
#include <functional>

using CollisionHandlerFunction = std::function<void(CollisionInformation, bool, entt::entity, entt::entity, entt::registry&)>;

/*
    Component that defines an entity's response to collision with another object.
    Holds a pointer to a function that defines its behavior. If an entity has
    a CollisionHandler it is assumed that the entity is not static.
*/
struct CollisionHandler{
    CollisionHandlerFunction handler;

    /*
        Checks if the handler should call its function. Gets reset to true every frame, but
        gets set to false if another CollisionHandler has handled the collision of this
        entity (for example, when the velocities are switched in an elastic collision, you
        don't want them to be switched again by the other entity, so you disable that entity's
        handler).
    */
    bool physicsHandlingEnabled;

    inline void disable_physics_handling(){
        physicsHandlingEnabled = false;
    }
};

struct DefaultElasticCollisionHandler {
    float elasticity = 0.9;
    DefaultElasticCollisionHandler(float elasticity);
    void operator()(CollisionInformation info, bool physicsHandlingEnabled, entt::entity entityThis, entt::entity entityOther, entt::registry& registry) const;
};

CollisionHandlerFunction join_handlers(CollisionHandlerFunction&& handler1, CollisionHandlerFunction&& handler2);

// Constructs a default collision handler with the respective default function and a given elasticity value.
CollisionHandler default_collision_handler(float elasticity = 0.9);