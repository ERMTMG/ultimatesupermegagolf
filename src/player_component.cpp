#include"player_component.h"
#include<cassert>

void update_input(InputManager& input){
    input.mouseScreenPosition = GetMousePosition();
    input.keysLastFrame = input.keys;
    input.keys[InputManager::LEFT]        = IsKeyDown(KEY_LEFT);
    input.keys[InputManager::RIGHT]       = IsKeyDown(KEY_RIGHT);
    input.keys[InputManager::DOWN]        = IsKeyDown(KEY_DOWN);
    input.keys[InputManager::UP]          = IsKeyDown(KEY_UP);
    input.keys[InputManager::PAUSE]       = IsKeyDown(KEY_ENTER);
    input.keys[InputManager::RESET]       = IsKeyDown(KEY_R);
    input.keys[InputManager::MOUSE_CLICK] = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

bool is_input_active(const InputManager& input, InputManager::Inputs check){
    assert(check < InputManager::INPUTS_ENUM_SIZE);
    return input.keys[check];
}

bool is_input_pressed_this_frame(const InputManager& input, InputManager::Inputs check){
    assert(check < InputManager::INPUTS_ENUM_SIZE);
    return (input.keys[check] && !input.keysLastFrame[check]);
}

bool is_input_released_this_frame(const InputManager& input, InputManager::Inputs check){
    assert(check < InputManager::INPUTS_ENUM_SIZE);
    return (!input.keys[check] && input.keysLastFrame[check]);
}

void update_player(PlayerComponent& player, Velocity& vel, const InputManager& input, const CameraView& camera){
    static const float MAX_IMPULSE_STRENGTH = 10 * M_2_PI; // don't take out the "*M_2_PI", it offsets the arctan's pi/2 limit
    static const float IMPULSE_STRENGTH_SMOOTHNESS = 5; // controls the graduality of the curve from 0 impulse strength to max impulse strength
    static const float VELOCITY_MARGIN_SQ = 0.01; // if the velocity's length is less than this value's square root then the player can drag again
    if(player.canDrag){
        if(is_input_pressed_this_frame(input, InputManager::MOUSE_CLICK)){
            Vector2 positionInWorld = GetScreenToWorld2D(input.mouseScreenPosition, camera.cam);
            player.mouseAnchorPosition = Position(positionInWorld);
        } else if(is_input_active(input, InputManager::MOUSE_CLICK)){
            Vector2 screenAnchor = GetWorldToScreen2D(to_Vector2(player.mouseAnchorPosition), camera.cam);
            Vector2 difference = input.mouseScreenPosition - screenAnchor;
            float totalStrength = MAX_IMPULSE_STRENGTH * atan(IMPULSE_STRENGTH_SMOOTHNESS * length(difference));
            player.potentialVelocity = totalStrength * unit_vector(difference);
        } else if(is_input_released_this_frame(input, InputManager::MOUSE_CLICK)){
            release_player_drag_velocity(player, vel);
        }

        player.canDrag = (length_squared(to_Vector2(vel)) < VELOCITY_MARGIN_SQ);

        //TODO later: do something with the health? still gotta program something that takes away health in the first place tho
    }
}

void release_player_drag_velocity(PlayerComponent& player, Velocity& vel){
    vel = Velocity(player.potentialVelocity);
    player.potentialVelocity = {0,0};
    player.mouseAnchorPosition = {0,0};
    player.canDrag = false;
    player.totalImpulses++;
}