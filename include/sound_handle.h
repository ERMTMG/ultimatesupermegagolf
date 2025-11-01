#pragma once
#include <raylib.h>
#include <cstddef>

class SoundHandle {
  private:
    static const size_t MAX_SOUND_COPIES = 8;
    Sound m_soundArray[MAX_SOUND_COPIES];
    size_t m_currentIdx;
    bool m_isSoundSource;
    bool m_holdsSoundData;
  public:
    SoundHandle() = delete;
    SoundHandle(const char* filepath);
    SoundHandle(const SoundHandle& other);
    SoundHandle(SoundHandle&&);
    SoundHandle& operator=(const SoundHandle& other);
    SoundHandle& operator=(SoundHandle&&);
    ~SoundHandle();

    void play();
    void stop_all();
    bool operator==(const SoundHandle& rhs) const;
    inline bool operator!=(const SoundHandle& rhs) const {
        return !(*this == rhs);
    }
};