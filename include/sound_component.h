/*
    FILE: sound_component.h
    Defines a component that manages sound playing for an entity.
    An entity can have multiple sounds associated via the same component.
*/
#pragma once
#include "sound_handle.h"
#include "sound_loader.h"

#include <vector>

// Sounds are indexed by a number for easy key comparison.
using SoundKey = size_t;
// This converts a string literal to a sound key so syntax like
//  try_play_sound(soundComponent, "hit"_sound);
// is valid.
SoundKey operator""_sound(const char* str, size_t str_size);

// Same as operator""_sound but for std::strings.
SoundKey to_key(const std::string& str);
// Same as oeprator""_sound
SoundKey to_key(const char* str);

// This component stores a set of sounds indexed by sound keys.
// The storage is done by a std::vector instead of an std::map or similar
// because most entities will have very few sounds so it would be overkill
struct SoundComponent {
    std::vector<SoundHandle> sounds;
    std::vector<SoundKey> keys;

    SoundComponent();
    ~SoundComponent();
};

// Adds a new sound to the component's indexing using the given key. The sound
// will be loaded from soundFilename.
void add_sound_to_component(SoundComponent& sound, const char* soundFilename, SoundKey key);

// Removes the sound indexed by the given sound key from the component. If the sound doesn't
// exist in the component, it does nothing.
void remove_sound_from_component(SoundComponent& sound, SoundKey key);

// Plays the sound indexed by the given sound key in the component, if it exists.
void try_play_sound(SoundComponent& sound, SoundKey key);

// Returns true only if the given sound component has the sound key passed as parameter.
bool has_sound(const SoundComponent& sound, SoundKey key);
