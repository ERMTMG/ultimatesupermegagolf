#include"collision_handler.h"

void default_elastic_collision_handler_fn(CollisionInformation info, Velocity* thisVelocity, Velocity* otherVelocity, CollisionHandler* otherHandler){
    static const float ELASTICITY = 0.9;
    
    if(thisVelocity != nullptr && otherVelocity != nullptr){
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
    }
}

CollisionHandler default_collision_handler(){
    return CollisionHandler { .defaultHandler = default_elastic_collision_handler_fn, COLLISION_HANDLER_DEFAULT, true };
}