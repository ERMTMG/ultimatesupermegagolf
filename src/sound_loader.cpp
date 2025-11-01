#include "sound_loader.h"
#include "sound_handle.h"
#include <cassert>
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace SoundLoader {

SoundInfo::SoundInfo(const char* filepath) : refCount(1), sound(filepath) {};

SoundHandle load_or_get_sound(const char* filepath){
    auto itr = _soundFileMap.find(filepath);
    if(itr != _soundFileMap.end()){
        SoundInfo& soundInfo = itr->second;
        soundInfo.refCount++;
        return SoundHandle{soundInfo.sound};
    } else {
        SoundInfo soundInfo{filepath};
        _soundFileMap.emplace(std::make_pair(filepath, std::move(soundInfo)));
        return SoundHandle{soundInfo.sound};
    }
}

void return_sound(SoundHandle& sound){
    auto itr = std::find_if(_soundFileMap.begin(), _soundFileMap.end(), [&sound](const std::pair<const std::string, SoundInfo>& pair){
        return pair.second.sound == sound;
    });

    if(itr != _soundFileMap.end()){
        itr->second.refCount--;
        if(itr->second.refCount == 0){
            _soundFileMap.erase(itr);
        }
    }
}

SoundHandle get_sound_copy(const SoundHandle& sound){
    auto itr = std::find_if(_soundFileMap.begin(), _soundFileMap.end(), [&sound](const std::pair<const std::string, SoundInfo>& pair){
        return pair.second.sound == sound;
    });

    if(itr != _soundFileMap.end()){
        itr->second.refCount++;
        return SoundHandle{itr->second.sound};
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
