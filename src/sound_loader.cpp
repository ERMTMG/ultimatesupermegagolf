#include "sound_loader.h"
#include "raylib.h"
#include <cassert>
#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace SoundLoader {

SoundInfo::SoundInfo(const char* filepath) : refCount(1), sound(LoadSound(filepath)), unloadOnDestruct(false) {};

SoundInfo::~SoundInfo(){
    if(unloadOnDestruct){
      assert("Unloading a sound with a reference count greater than 0" && refCount > 0);
      UnloadSound(this->sound);
    }
}

Sound load_or_get_sound(const char* filepath){
    auto itr = _soundFileMap.find(filepath);
    if(itr != _soundFileMap.end()){
        SoundInfo& soundInfo = itr->second;
        soundInfo.refCount++;
        return LoadSoundAlias(soundInfo.sound);
    } else {
        SoundInfo soundInfo{filepath};
        _soundFileMap.emplace(std::make_pair(filepath, std::move(soundInfo)));
        _soundFileMap[filepath].unloadOnDestruct = true;
        return LoadSoundAlias(soundInfo.sound);
    }
}

static bool operator==(const Sound& lhs, const Sound& rhs){
    return lhs.stream.buffer == rhs.stream.buffer;
}

void return_sound(Sound sound){
    auto itr = std::find_if(_soundFileMap.begin(), _soundFileMap.end(), [&sound](const std::pair<std::string, SoundInfo>& pair){
        return pair.second.sound == sound;
    });

    if(itr != _soundFileMap.end()){
        itr->second.refCount--;
        if(itr->second.refCount == 0){
            _soundFileMap.erase(itr);
        }
    }
}

Sound get_sound_copy(Sound sound){
    auto itr = std::find_if(_soundFileMap.begin(), _soundFileMap.end(), [&sound](const std::pair<std::string, SoundInfo>& pair){
        return pair.second.sound == sound;
    });

    if(itr != _soundFileMap.end()){
        itr->second.refCount++;
        return LoadSoundAlias(sound);
    } else {
        throw new std::invalid_argument("Copying sound not registered by SoundLoader");
    }
}

size_t _get_sound_ref_count(const char* filepath){
    auto iter = _soundFileMap.find(filepath);
    if(iter != _soundFileMap.end()){
        return iter->second.refCount;
    } else {
        return 0;
    }
}

} // namespace SoundLoader
