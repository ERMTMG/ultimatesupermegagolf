#include"particles.h"
#include "raylib.h"

Particle::Particle(
    const Color& color,
    const Vector2& relativePos,
    const Vector2& velocity,
    const Vector2& acceleration,
    float angle,
    float rotationalVelocity,
    float lifetime
) : m_color(color),
    m_relativePos(relativePos),
    m_velocity(velocity),
    m_acceleration(acceleration),
    m_currentAngle(angle),
    m_rotationalVelocity(rotationalVelocity),
    m_timeToLive(lifetime) {};


void Particle::update(float delta){
    m_velocity += m_acceleration * delta;
    m_relativePos += m_velocity * delta;
    m_currentAngle += m_rotationalVelocity * delta;
    m_timeToLive -= delta;
}

bool Particle::shouldDie() const {
    return (m_timeToLive <= 0.0f);
}

void Particle::draw(const Texture& texture, const Vector2& position) const {
    int width = texture.width;
    int height = texture.height;
    Rectangle rectFrom = Rectangle {0, 0, width, height};
    Rectangle rectTo = Rectangle {
        .x = -width/2.f,
        .y = -height/2.f,
        .width = width,
        .height = height
    };
    DrawTexturePro(texture, rectFrom, rectTo, position + m_relativePos, m_currentAngle, m_color);
}
