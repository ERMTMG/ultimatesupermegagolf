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

static const float MAX_IMPULSE_STRENGTH = 300 * M_2_PI; // don't take out the "*M_2_PI", it offsets the arctan's pi/2 limit
static const float IMPULSE_STRENGTH_SMOOTHNESS = 100; // controls the graduality of the curve from 0 impulse strength to max impulse strength
static const float VELOCITY_MARGIN_SQ = 2; // if the velocity's length is less than this value's square root then the player can drag again
static const float MIN_GROUND_RESISTANCE = 0.99;
static const float MAX_GROUND_RESISTANCE = 0.93;

void update_player(PlayerComponent& player, Velocity& vel, const InputManager& input, const CameraView& camera){
    if(player.canDrag){
        if(is_input_pressed_this_frame(input, InputManager::MOUSE_CLICK)){
            Vector2 positionInWorld = GetScreenToWorld2D(input.mouseScreenPosition, camera.cam);
            player.mouseAnchorPosition = Position(positionInWorld);
        } else if(is_input_active(input, InputManager::MOUSE_CLICK)){
            Vector2 screenAnchor = GetWorldToScreen2D(to_Vector2(player.mouseAnchorPosition), camera.cam);
            Vector2 difference = input.mouseScreenPosition - screenAnchor;
            float totalStrength = MAX_IMPULSE_STRENGTH * atan( length(difference)/IMPULSE_STRENGTH_SMOOTHNESS);
            player.potentialVelocity = -totalStrength * unit_vector(difference);
        } else if(is_input_released_this_frame(input, InputManager::MOUSE_CLICK)){
            release_player_drag_velocity(player, vel);
        }

        
        //TODO later: do something with the health? still gotta program something that takes away health in the first place tho
    }
    float groundResistance = (length(to_Vector2(vel)) > 10) ? MIN_GROUND_RESISTANCE : MAX_GROUND_RESISTANCE;
    vel = {groundResistance*vel.v_x, groundResistance*vel.v_y};
    player.canDrag = (length_squared(to_Vector2(vel)) < VELOCITY_MARGIN_SQ);
}

void release_player_drag_velocity(PlayerComponent& player, Velocity& vel){
    std::cout << "player velocity released!: " << player.potentialVelocity << '\n';
    if(!is_vector2_nan(player.potentialVelocity)){
        vel = Velocity(player.potentialVelocity);
        player.potentialVelocity = VEC2_ZERO;
        player.mouseAnchorPosition = {0,0};
        player.canDrag = false;
        player.totalImpulses++;
    }
}

void draw_player_drag_velocity(const PlayerComponent &player, const Position& pos){
    static const float LOW_HUE = 200;
    static const int ARROW_THICKNESS = 3;
    static const int ARROW_TIP_SIZE = 5;
    static const float ARROW_SIZE_REDUCTION_FACTOR = 2.5;
    static const unsigned char ARROW_ALPHA = 96;

    Vector2 arrowLength = player.potentialVelocity / ARROW_SIZE_REDUCTION_FACTOR;
    float l = length(arrowLength);
    if(l > 0.01){
        float hue = LOW_HUE - LOW_HUE * l/(MAX_IMPULSE_STRENGTH * M_PI_2 / ARROW_SIZE_REDUCTION_FACTOR);
        Color color = ColorFromHSV(hue, 1, 1);
        color.a = ARROW_ALPHA;
        draw_arrow(to_Vector2(pos), to_Vector2(pos)+arrowLength, color, ARROW_TIP_SIZE, ARROW_THICKNESS);
    }
}
