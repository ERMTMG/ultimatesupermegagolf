/*
    FILE: player_component.h 
    Defines components to manage input and to represent player-specific variables. 
*/
#pragma once
#include"raylib.h"
#include<bitset>
#include"utility.h"
#include"basic_components.h"
#include"collision_component.h"
#include"camera_view.h"

/*
    Stores all the input information needed by the game: keys pressed, mouse info, etc.
    Could potentially support customizable keybinds? 
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

// Uodates the given input manager, setting the input map to the keys pressed this instant.
void update_input(InputManager& input);
// Checks and returns whether the given input is currently active (being held down).
bool is_input_active(const InputManager& input, InputManager::Inputs check);
// Checks and returns whether the given input has been activated this frame (i.e, if it wasn't
// active last frame but it is this frame).
bool is_input_pressed_this_frame(const InputManager& input, InputManager::Inputs check);
// Checks and returns whether the given input has been released this frame (i.e, if it was active
// last frame and it isn't this frame).
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
    static constexpr HPType MAX_HEALTH = 100;
    HPType health = MAX_HEALTH;
    bool canDrag;
};

// Updates the PlayerComponent's info and its associated velocity, taking into account the current
// input. The camera is necessary to transform from mouse position to in-world position.
void update_player(PlayerComponent& player, Velocity& vel, const InputManager& input, const CameraView& camera);

// Called when the player releases the mouse, intending to give an impulse to the player.
// Calculates the impulse based on the mouse movement and sets the player's velocity.
void release_player_drag_velocity(PlayerComponent& player, Velocity& vel);

// Draws an arrow to the screen symbolizing the player's potential impulse if they release mouse.
void draw_player_drag_velocity(const PlayerComponent& player, const Position& pos);
