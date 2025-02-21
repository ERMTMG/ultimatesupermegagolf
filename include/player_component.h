#pragma once
#include"raylib.h"
#include<bitset>
#include"vector2_utilities.h"
#include"basic_components.h"
#include"collision_component.h"
#include"camera_view.h"

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
    typedef std::bitset<INPUTS_ENUM_SIZE> InputMap;
    InputMap keys = 0;
    InputMap keysLastFrame = 0;
};

void update_input(InputManager& input);
bool is_input_active(const InputManager& input, InputManager::Inputs check);
bool is_input_pressed_this_frame(const InputManager& input, InputManager::Inputs check);
bool is_input_released_this_frame(const InputManager& input, InputManager::Inputs check);


typedef unsigned char HPType;
/*
Stores variable information about the player: whether it can receive an impulse yet, 
the current potential velocity it has, the number of times it's been impulsed, etc.
*/
struct PlayerComponent{
    Position mouseAnchorPosition;
    Vector2 potentialVelocity;
    unsigned int totalImpulses = 0;
    static const HPType MAX_HEALTH = 100;
    HPType health = MAX_HEALTH;
    bool canDrag;
};

void update_player(PlayerComponent& player, Velocity& vel, const InputManager& input, const CameraView& camera);
void release_player_drag_velocity(PlayerComponent& player, Velocity& vel);

