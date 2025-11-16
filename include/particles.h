#include<raylib.h>
#include"utility.h"

class Particle {
  private:
    Color m_color;
    Vector2 m_relativePos;
    Vector2 m_velocity;
    Vector2 m_acceleration;
    float m_currentAngle; // in degrees!
    float m_rotationalVelocity;
    float m_timeToLive;
    Particle();
  public:
    Particle(
        const Color& color,
        const Vector2& relativePos,
        const Vector2& velocity,
        const Vector2& acceleration,
        float angle,
        float rotationalVelocity,
        float lifetime
    );
    Particle(const Particle&) = default;
    Particle(Particle&&) = default;
    Particle& operator=(const Particle& other) = default;
    Particle& operator=(Particle&& other) = default;
    void update(float delta);
    bool shouldDie() const;
    void draw(const Texture& texture, const Vector2& position) const;
};
