#include"collision_handler.h"
#include "basic_components.h"
#include "collision_shapes.h"
#include "raylib.h"
#include "utility/vector2_util.h"
#include <utility>

void default_elastic_collision_handler_fn(CollisionInformation info, Velocity* thisVelocity, Velocity* otherVelocity, CollisionHandler* otherHandler){
    static const float ELASTICITY = 0.9;
    
    if(thisVelocity != nullptr && otherVelocity != nullptr && info.collision){ // case 1: both objects are not static and are colliding
        Velocity& vel1 = *thisVelocity;
        vel1.v_x *= ELASTICITY; vel1.v_y *= ELASTICITY;
        Velocity& vel2 = *otherVelocity;
        vel2.v_x *= ELASTICITY; vel2.v_y *= ELASTICITY;
        Velocity aux = vel1;
        vel1 = vel2;
        vel2 = aux;
        if(otherHandler != nullptr){
            otherHandler->disable_physics_handling();
        }
    } else if(thisVelocity != nullptr && info.collision){ // case 2: only calling object is not static, colliding with some other static object
        Velocity& vel = *thisVelocity;
        Vector2 reflectedVelocity = ELASTICITY * reflect_across_normal(to_Vector2(vel), info.unitNormal);
        vel = Velocity(reflectedVelocity);
    }
}

DefaultElasticCollisionHandler::DefaultElasticCollisionHandler(float elasticity) : elasticity(elasticity) {};

void DefaultElasticCollisionHandler::operator()(CollisionInformation info, bool physicsHandlingEnabled, entt::entity entityThis, entt::entity entityOther, entt::registry& registry) const {
    if(!info.collision) return;
    if(!physicsHandlingEnabled) return; // This only manages physics
    Velocity* thisVelocity = registry.try_get<Velocity>(entityThis);
    Velocity* otherVelocity = registry.try_get<Velocity>(entityOther);
    CollisionHandler* otherHandler = registry.try_get<CollisionHandler>(entityOther);
    if(thisVelocity != nullptr && otherVelocity != nullptr){
        thisVelocity->v_x *= this->elasticity; thisVelocity->v_y *= this->elasticity;
        otherVelocity->v_x *= this->elasticity; otherVelocity->v_y *= this->elasticity;
        std::swap(*thisVelocity, *otherVelocity);
        if(otherHandler != nullptr){
            otherHandler->disable_physics_handling();
        } 
    } else if(thisVelocity != nullptr){
        Vector2 reflectedVelocity = this->elasticity * reflect_across_normal(to_Vector2(*thisVelocity), info.unitNormal);
        *thisVelocity = Velocity(reflectedVelocity);
    }
} 

CollisionHandlerFunction join_handlers(CollisionHandlerFunction&& handler1, CollisionHandlerFunction&& handler2){
    return [handler1 = std::move(handler1), handler2 = std::move(handler2)](
        CollisionInformation info, bool physicsHandlingEnabled, entt::entity entityThis, entt::entity entityOther, entt::registry& registry
    ) -> void {
        handler1(info, physicsHandlingEnabled, entityThis, entityOther, registry);
        handler2(info, physicsHandlingEnabled, entityThis, entityOther, registry);
    };
}

CollisionHandler default_collision_handler(float elasticity){
    return CollisionHandler{
        .handler = DefaultElasticCollisionHandler{elasticity},
        .physicsHandlingEnabled = true
    };
}