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

/*
    Component that defines an entity's response to collision with another object.
    Holds a pointer to a function that defines its behavior. If an entity has
    a CollisionHandler it is assumed that the entity is not static.
*/
struct CollisionHandler{
    union{
        /*
            Default type of collision handler. Needs a reference to the own entity's velocity,
            the other entity's velocity and the other entity's collisionHandler (to maybe disable it 
            for the rest of the frame in case the collision handler swaps velocities). More efficient 
            than the custom handler because it has direct access to the entities' needed components.
        */
        void (*defaultHandler)(CollisionInformation, Velocity*, Velocity*, CollisionHandler*);

        // Custom handler that has the entities' address in the registry for access to other components.
        // Of course, allows for much more complex behavior but needs to individually get every other component.
        void (*customHandler)(CollisionInformation, entt::entity, entt::entity, entt::registry*);
    };

    bool type;
        #define COLLISION_HANDLER_DEFAULT true
        #define COLLISION_HANDLER_CUSTOM false

    /*
        Checks if the handler should call its function. Gets reset to true every frame, but
        gets set to false if another CollisionHandler has handled the collision of this
        entity (for example, when the velocities are switched in an elastic collision, you
        don't want them to be switched again by the other entity, so you disable that entity's
        handler).
    */
    bool enabled;

    inline void disable_if_default(){
        if(type == COLLISION_HANDLER_DEFAULT){
            enabled = false;
        }
    }
};

// Defines the internal function for the default collision handler
void default_elastic_collision_handler_fn(CollisionInformation info, Velocity* thisVelocity, Velocity* otherVelocity, CollisionHandler* otherHandler);

// Constructs a default collision handler with the respective default function
CollisionHandler default_collision_handler();