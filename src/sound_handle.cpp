#include "sound_handle.h"
#include "raylib.h"
#include <cstddef>

SoundHandle::SoundHandle(const char* filepath){
    m_soundArray[0] = LoadSound(filepath);
    for(size_t i = 1; i < MAX_SOUND_COPIES; i++){
        m_soundArray[i] = LoadSoundAlias(m_soundArray[0]);
    }
    m_currentIdx = 0;
    isSoundSource = true;
}

SoundHandle::SoundHandle(const SoundHandle& other){
    for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
        this->m_soundArray[i] = LoadSoundAlias(other.m_soundArray[0]);
    }
    m_currentIdx = 0;
    isSoundSource = false;
}

SoundHandle& SoundHandle::operator=(const SoundHandle& other){
    if(this != &other){
        for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
            this->m_soundArray[i] = LoadSoundAlias(other.m_soundArray[0]);
        }
        m_currentIdx = 0;
        isSoundSource = false;
    }
    return *this;
}

SoundHandle::~SoundHandle(){
    if(isSoundSource){
        for(size_t i = 1; i < MAX_SOUND_COPIES; i++){
            UnloadSoundAlias(m_soundArray[i]);
        }
        UnloadSound(m_soundArray[0]);
    } else {
        for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
            UnloadSoundAlias(m_soundArray[i]);
        }
    }
}

void SoundHandle::play(){
    PlaySound(m_soundArray[m_currentIdx]);
    m_currentIdx = (m_currentIdx + 1) % MAX_SOUND_COPIES;
}

void SoundHandle::stop_all(){
    for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
        if(IsSoundPlaying(m_soundArray[i])){
            StopSound(m_soundArray[i]);
        }
    }
}