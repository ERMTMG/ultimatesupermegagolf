#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
#include "sound_handle.h"

namespace SoundLoader {

struct SoundInfo {
    size_t refCount;
    SoundHandle sound;
    SoundInfo() = delete;
    SoundInfo(const char* filepath);
    SoundInfo(SoundInfo&&) = default;
};

inline std::unordered_map<std::string, SoundInfo> _soundFileMap;

SoundHandle load_or_get_sound(const char* filepath);
void return_sound(SoundHandle& sound);
SoundHandle get_sound_copy(const SoundHandle& sound);
size_t _get_sound_ref_count(const char* filepath);

}
