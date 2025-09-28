#include <raylib.h>
#include <cstddef>

class SoundHandle {
  private:
    static const size_t MAX_SOUND_COPIES = 8;
    Sound m_soundArray[MAX_SOUND_COPIES];
    size_t m_currentIdx;
    bool isSoundSource;
  public:
    SoundHandle() = delete;
    SoundHandle(const char* filepath);
    SoundHandle(const SoundHandle& other);
    SoundHandle(SoundHandle&&) = default;
    SoundHandle& operator=(const SoundHandle& other);
    SoundHandle& operator=(SoundHandle&&) = default;
    ~SoundHandle();
    void play();
    void stop_all();
};