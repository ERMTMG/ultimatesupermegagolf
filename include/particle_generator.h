/*
    FILE: particle_generator.h
    Defines a component that acts as a particle generator, where each particle is
    rendered without being its own entiity.
*/
#include"basic_components.h"
#include"raylib.h"
#include"particles.h"
#include"rng_component.h"
#include"utility.h"
#include "utility/random_range.h"

#include<vector>

// This struct defines the settings with which particles can spawn. Each range property
// selects a random value within the range for each particle spawned.
struct ParticleSettings {
    Texture2D texture; // The texture each particle should have.
    Rectangle spawningArea; // The rectangle within which all particles will spawn randomly
    Vec2Range initialVelocity; // The particles' velocity upon spawning
    Vector2 acceleration; // A constant acceleration (e.g. gravity) for all particles
    FloatRange lifetime; // The time, in seconds, that particles should live before despawning
    FloatRange spawnPeriod; // The time, in seconds, between consecutive spawns
    IntRange spawnQuantity; // The number of particles spawned each time a spawnPeriod ends
    FloatRange initialRotation; // The initial rotation, in degrees, that a particle will have on spawn
    FloatRange rotationalVelocity; // Constant rotational velocity on each particle
    ColorRange color; // The color applied to the texture upon drawing. Will linearly fade between the colors in the range
};

// This is the component in question.
struct ParticleGenerator {
    ParticleSettings settings;
    std::vector<Particle> particlePool;
    float particleCurrentSpawnTimer; // Timer is negative if disabled. Keeps track of current spawn time
};

// Constructs a new particle generator with the given settings.
ParticleGenerator new_particle_generator(ParticleSettings&& settings);

// Enables the given particle generator if it was disabled.
void particle_generator_enable(ParticleGenerator& particles);

// Disables the given particle generator if it was enabled.
void particle_generator_disable(ParticleGenerator& particles);

// Resets the given particle generator, erasing all particles.
void particle_generator_reset(ParticleGenerator& particles);

// Updates all particles in the given generator's pool (lifetimes, velocities, etc.), spawns new
// particles if needed and erases particles whose lifetimes have ended. An RNG component is needed
// for new particles spawned.
void particle_generator_update(ParticleGenerator& particles, RNGComponent& rng, float delta);

// Draws all particles in the given generator's pool with the given position offset.
void particle_generator_draw(const ParticleGenerator& particles, const Position& pos = {0,0});

// Returns the number of particles in the given generator's pool.
size_t particle_generator_current_number_of_particles(const ParticleGenerator& particles);
