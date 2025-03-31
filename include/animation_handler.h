/*
    FILE: animation_handler.h
    This file defines the AnimationHandler component, which contains the animation
    logic for entities, as well as a default animation handler which just cycles through
    the frames of a spritesheet, though it is possible to define more complex animation logic.
*/
#pragma once
#include"raylib.h"
#include"basic_components.h"

// Main animation handler struct. Static (one frame) sprites are not recommended to have an 
// AnimationHandler component.
struct AnimationHandler{
    /*
        Function pointer that defines the animation logic for the current entity. Takes in:
            - Argument 1 (float) delta time for the current frame
            - Argument 2 (float&) reference to the component's internal timer
            - Argument 3 (SpriteSheet&) reference to the entity's spritesheet component
        Internal logic can be as simple as a periodic frame switcher or as complex as a custom behavior
        for each available animation. 
    */
    void (*handler)(float, float&, SpriteSheet&);
    // Internal timer that displays the sum of all delta-times until the current moment.
    // Should be periodically reset to avoid floating point inaccuracies stemming from 
    // it getting too large.
    float timer = 0;
};

// Internal logic for the default animation handler. Cycles through the animation frames of the
// spritesheet at a constant rate.
template<int FRAME_DURATION_MILLISECONDS>
void default_animation_handler_fn(float delta, float& timer, SpriteSheet& sprite){
    static const float FRAME_DURATION_SECONDS = FRAME_DURATION_MILLISECONDS / 1000.0;
    timer += delta;
    if(timer > FRAME_DURATION_SECONDS){
        timer -= FRAME_DURATION_SECONDS;
        next_frame(sprite);
    }
}

// Constructs an AnimationHandler component from the default function.
template<int FRAME_DURATION_MILLISECONDS = 166>
AnimationHandler default_animation_handler(){
    return AnimationHandler {&default_animation_handler_fn<FRAME_DURATION_MILLISECONDS>, 0} ;
}