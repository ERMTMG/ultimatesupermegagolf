#pragma once
#include "collision_handler.h"
#include "collision_shapes.h"
#include "sound_component.h"

struct PlaySoundCollisionHandler {
    SoundKey playedSoundKey;
    PlaySoundCollisionHandler(SoundKey playedSoundKey);
    void operator() (CollisionInformation info, bool physicsHandlingEnabled, entt::entity entityThis, entt::entity entityOther, entt::registry& registry) const;
};