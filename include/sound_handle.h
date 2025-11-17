/*
    FILE: sound_handle.h
    Defines a class that abstracts sound playing such that a single sound can be played
    polyphonically.
*/
#pragma once
#include <raylib.h>
#include <cstddef>

// An abstraction to play a given singular sound more than one time.
// Effectively a wrapper over an array of aliases of the same sound.
class SoundHandle {
  private:
    static const size_t MAX_SOUND_COPIES = 8;
    Sound m_soundArray[MAX_SOUND_COPIES];
    size_t m_currentIdx;
    bool m_isSoundSource; // This flag controls whether the original sound is actually loaded by this handle object (else, all sounds in the array are aliases)
    bool m_holdsSoundData; // This flag controls whether sounds need to be unloaded or they have been moved away
  public:
    // Can't construct without any arguments.
    SoundHandle() = delete;
    // Loads a sound from the given filepath, loading the rest of the sound as sound aliases.
    SoundHandle(const char* filepath);
    SoundHandle(const SoundHandle& other);
    SoundHandle(SoundHandle&&);
    SoundHandle& operator=(const SoundHandle& other);
    SoundHandle& operator=(SoundHandle&&);
    ~SoundHandle();

    // Plays the sound, with polyphony up to MAX_SOUND_COPIES (8)
    void play();
    // Stops all the currently playing sounds. Does nothing if no sounds are playing
    void stop_all();
    // Compares the audio buffer of both sounds, checking if they point to the same data.
    bool operator==(const SoundHandle& rhs) const;
    inline bool operator!=(const SoundHandle& rhs) const {
        return !(*this == rhs);
    }
};
