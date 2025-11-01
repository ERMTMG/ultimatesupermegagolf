#include "sound_handle.h"
#include "raylib.h"
#include <cstddef>
#include <iostream>

SoundHandle::SoundHandle(const char* filepath){
    m_soundArray[0] = LoadSound(filepath);
    for(size_t i = 1; i < MAX_SOUND_COPIES; i++){
        m_soundArray[i] = LoadSoundAlias(m_soundArray[0]);
    }
    m_currentIdx = 0;
    m_isSoundSource = true;
    m_holdsSoundData = true;
}

SoundHandle::SoundHandle(const SoundHandle& other){
    for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
        this->m_soundArray[i] = LoadSoundAlias(other.m_soundArray[0]);
    }
    m_currentIdx = 0;
    m_isSoundSource = false;
    m_holdsSoundData = true;
}

SoundHandle::SoundHandle(SoundHandle&& other){
    for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
        this->m_soundArray[i] = other.m_soundArray[i];
    }
    this->m_currentIdx = 0;
    this->m_isSoundSource = other.m_isSoundSource;
    this->m_holdsSoundData = other.m_holdsSoundData;
    other.m_holdsSoundData = false;
}

SoundHandle& SoundHandle::operator=(const SoundHandle& other){
    if(this != &other){
        for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
            this->m_soundArray[i] = LoadSoundAlias(other.m_soundArray[0]);
        }
        m_currentIdx = 0;
        m_isSoundSource = false;
        m_holdsSoundData = true;
    }
    return *this;
}

SoundHandle& SoundHandle::operator=(SoundHandle&& other){
    if(this != &other){
        for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
            this->m_soundArray[i] = other.m_soundArray[i];
        }
        this->m_currentIdx = 0;
        this->m_isSoundSource = other.m_isSoundSource;
        this->m_holdsSoundData = other.m_holdsSoundData;
        other.m_holdsSoundData = false;
    }
    return *this;
}

SoundHandle::~SoundHandle(){
    std::cout << "SoundHandle destructor called\n";
    if(m_holdsSoundData){
        if(m_isSoundSource){
            for(size_t i = 1; i < MAX_SOUND_COPIES; i++){
                std::cout << "\tUnloading sound alias " << i << "...\n";
                UnloadSoundAlias(m_soundArray[i]);
            }
            std::cout << "\tUnloading original sound...\n";
            UnloadSound(m_soundArray[0]);
        } else {
            for(size_t i = 0; i < MAX_SOUND_COPIES; i++){
                std::cout << "\tUnloading sound alias " << i << "...\n";
                UnloadSoundAlias(m_soundArray[i]);
            }
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

bool SoundHandle::operator==(const SoundHandle& rhs) const {
    return (this->m_soundArray[0].stream.buffer == this->m_soundArray[0].stream.buffer);
}