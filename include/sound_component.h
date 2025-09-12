#pragma once
#include <raylib.h>
#include "sound_loader.h"

#include <vector>

using SoundKey = size_t;
SoundKey operator""_sound(const char* str, size_t str_size);

SoundKey to_key(const std::string& str);
SoundKey to_key(const char* str);

// TODO: is there a better way to do this?
struct SoundComponent {
    std::vector<Sound> sounds;
    std::vector<SoundKey> keys;

    SoundComponent();
};

void add_sound_to_component(SoundComponent& sound, const char* soundFilename, SoundKey key);

void remove_sound_from_component(SoundComponent& sound, SoundKey key);

void try_play_sound(const SoundComponent& sound, SoundKey key);

bool has_sound(const SoundComponent& sound, SoundKey key);
