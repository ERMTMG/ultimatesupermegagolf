#include "custom_collision_handlers.h"
#include "collision_handler.h"
#include "collision_shapes.h"
#include "sound_component.h"

PlaySoundCollisionHandler::PlaySoundCollisionHandler(SoundKey playedSoundKey) : playedSoundKey(playedSoundKey) {};

void PlaySoundCollisionHandler::operator() (CollisionInformation info, bool physicsHandlingEnabled, entt::entity entityThis, entt::entity entityOther, entt::registry& registry) const {
    if(!info.collision) return;
    SoundComponent* soundComponent = registry.try_get<SoundComponent>(entityThis);
    if(soundComponent != nullptr){
        try_play_sound(*soundComponent, this->playedSoundKey);
    }
}