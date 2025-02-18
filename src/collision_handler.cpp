#include"collision_handler.h"

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
            otherHandler->disable_if_default();
        }
    } else if(thisVelocity != nullptr && info.collision){ // case 2: only calling object is not static, colliding with some other static object
        Velocity& vel = *thisVelocity;
        Vector2 reflectedVelocity = ELASTICITY * reflect_across_normal(to_Vector2(vel), info.unitNormal);
        vel = Velocity(reflectedVelocity);
    }
}

CollisionHandler default_collision_handler(){
    return CollisionHandler { .defaultHandler = default_elastic_collision_handler_fn, COLLISION_HANDLER_DEFAULT, true };
}