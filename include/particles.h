#include<raylib.h>
#include"utility.h"

class Particle {
  private:
    const Color color;
    Vector2 m_relativePos;
    Vector2 m_velocity;
    const Vector2 m_acceleration;
    float m_currentAngle; // in degrees!
    const float m_rotationalVelocity;
    float timeToLive;
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
    void update(float delta);
    bool shouldDie() const;
    void draw() const;
};
