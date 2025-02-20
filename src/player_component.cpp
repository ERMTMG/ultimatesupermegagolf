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