/*
    FILE: sound_loader.h
    Provides a sound loading module, maintaining loaded sound handles
    to avoid reloading sounds unnnecessarily
*/
#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
#include "sound_handle.h"

namespace SoundLoader {

// This struct maintains info about a specific sound handle.
struct SoundInfo {
    size_t refCount;
    SoundHandle sound;
    SoundInfo() = delete;
    SoundInfo(const char* filepath);
    SoundInfo(const SoundInfo&) = default;
    SoundInfo(SoundInfo&&) = default;
};

inline std::unordered_map<std::string, SoundInfo> _soundFileMap;

// Searches the sound filepath in the internal sound file map. Returns a copy
// of the attached sound handle if it's found. If the sound is not loaded yet,
// it loads it and returns the handle.
SoundHandle load_or_get_sound(const char* filepath);

// Decreases the sound's reference count, unloading it if it reaches zero.
// It has to search linearly through the whole map so this should be done within loadtimes.
void return_sound(SoundHandle& sound);

// Searches the given sound handle in the map linearly. If it exists, the function
// returns a copy of the sound. If it doesn't, it throws an exception. Also shouldn't be
// done often.
SoundHandle get_sound_copy(const SoundHandle& sound);

// Returns the reference count associated to the sound with the given filename.
size_t _get_sound_ref_count(const char* filepath);

}
