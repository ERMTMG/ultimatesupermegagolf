#include"animation_handler.h"

template<int FRAME_DURATION_MILLISECONDS>
void default_animation_handler_fn(float delta, float& timer, SpriteSheet& sprite){
    static const float FRAME_DURATION_SECONDS = FRAME_DURATION_MILLISECONDS / 1000.0;
    timer += delta;
    if(timer > FRAME_DURATION_SECONDS){
        timer -= FRAME_DURATION_SECONDS;
        next_frame(sprite);
    }
}

template<int FRAME_DURATION_MILLISECONDS = 166>
AnimationHandler default_animation_handler(){
    return AnimationHandler {&default_animation_handler_fn<FRAME_DURATION_MILLISECONDS>, 0} ;
}
