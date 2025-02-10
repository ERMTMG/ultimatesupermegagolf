#pragma once
#include"raylib.h"
#include"vector2_utilities.h"
#include"basic_components.h"
#include"collision_component.h"

/*
Stores input and variable information about the player: whether it can receive an impulse yet, 
the current potential velocity it has, the number of times it's been impulsed, etc.
*/
struct PlayerComponent{
    Vector2 potentialVelocity;
    unsigned int totalImpulses;
    bool canDrag;
};

