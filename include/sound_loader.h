#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>

namespace SoundLoader {

struct SoundInfo {
    size_t refCount;
    Sound sound;
    SoundInfo() = default;
    SoundInfo(const char* filepath);
    SoundInfo(SoundInfo&&) = default;
    ~SoundInfo();
  private:
    bool unloadOnDestruct = false;
    friend Sound load_or_get_sound(const char*);
};

inline std::unordered_map<std::string, SoundInfo> _soundFileMap;

Sound load_or_get_sound(const char* filepath);
void return_sound(Sound sound);
Sound get_sound_copy(Sound sound);
size_t _get_sound_ref_count(const char* filepath);

}
