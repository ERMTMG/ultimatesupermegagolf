#pragma once
#include"raylib.h"
#include<bitset>
#include"vector2_utilities.h"
#include"basic_components.h"
#include"collision_component.h"

/*
Stores all the input information needed by the game: keys pressed, mouse info, etc
*/
struct InputManager{
    Vector2 mouseScreenPosition;
    enum Inputs{
        MOUSE_CLICK =      0,
        PAUSE,          // 1
        LEFT,           // 2
        RIGHT,          // 3
        UP,             // 4
        DOWN,           // 5
        RESET,          // 6
        INPUTS_ENUM_SIZE// 7
    };
    std::bitset<INPUTS_ENUM_SIZE> keys;
    std::bitset<INPUTS_ENUM_SIZE> keysLastFrame;
};

void update_input(InputManager& input);
bool is_input_active(const InputManager& input, InputManager::Inputs check);
bool is_input_pressed_this_frame(const InputManager& input, InputManager::Inputs check);
bool is_input_released_this_frame(const InputManager& input, InputManager::Inputs check);

/*
Stores variable information about the player: whether it can receive an impulse yet, 
the current potential velocity it has, the number of times it's been impulsed, etc.
*/
struct PlayerComponent{
    Vector2 mouseAnchorPosition;
    Vector2 potentialVelocity;
    unsigned int totalImpulses;
    bool canDrag;
};

