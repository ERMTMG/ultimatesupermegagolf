#include"raylib.h"
#include"particles.h"
#include "utility.h"
#include "utility/random_range.h"

#include <cstddef>
#include<vector>

struct ParticleSettings {
    Texture2D texture;
    Rectangle spawningArea;
    Vec2Range initialVelocity;
    Vector2 acceleration;
    FloatRange lifetime;
    FloatRange spawnPeriod;
    IntRange spawnQuantity;
    FloatRange initialRotation;
    FloatRange rotationalVelocity;
};

struct ParticleGenerator {
    ParticleSettings settings;
    std::vector<Particle> particlePool;
    float particleCurrentSpawnTimer; // Timer is negative if disabled
};

ParticleGenerator new_particle_generator(ParticleSettings settings);

void particle_generator_enable(ParticleGenerator& particles);

void particle_generator_disable(ParticleGenerator& particles);

void particle_generator_reset(ParticleGenerator& particles);

void particle_generator_update(ParticleGenerator& particles);

void particle_generator_draw(ParticleGenerator& particles);

size_t particle_generator_current_number_of_particles(ParticleGenerator& particles);
