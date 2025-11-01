#include "sound_component.h"
#include <algorithm>
#include <cstddef>
#include <cstring>

SoundKey operator""_sound(const char* str, size_t str_size){
    return std::hash<std::string>()({str, str_size});
}

SoundKey to_key(const std::string& str){
    return std::hash<std::string>()(str);
}

SoundKey to_key(const char* str){
    return operator""_sound(str, strlen(str));
}

SoundComponent::SoundComponent() : sounds{}, keys{} {
    static const size_t DEFAULT_CAPACITY = 4;
    sounds.reserve(DEFAULT_CAPACITY);
    keys.reserve(DEFAULT_CAPACITY);
}

void add_sound_to_component(SoundComponent& sound, const char* soundFilename, SoundKey key){
    SoundHandle loadedSound = SoundLoader::load_or_get_sound(soundFilename);
    sound.sounds.push_back(loadedSound);
    sound.keys.push_back(key);
}

void remove_sound_from_component(SoundComponent& sound, SoundKey key){
    auto itr = std::find(sound.keys.begin(), sound.keys.end(), key);
    if(itr != sound.keys.end()){
        size_t idx = itr - sound.keys.begin();
        auto sound_itr = sound.sounds.begin() + idx;
        sound.sounds.erase(sound_itr);
        sound.keys.erase(itr);
    }
}

void try_play_sound(SoundComponent& sound, SoundKey key){
    auto itr = std::find(sound.keys.begin(), sound.keys.end(), key);
    if(itr != sound.keys.end()){
        size_t idx = itr - sound.keys.begin();
        sound.sounds[idx].play();
    }
}

bool has_sound(const SoundComponent& sound, SoundKey key){
    return std::find(sound.keys.begin(), sound.keys.end(), key) != sound.keys.end();
}
