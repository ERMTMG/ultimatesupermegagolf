#include"particle_generator.h"
#include"basic_components.h"
#include"raylib.h"
#include "rng_component.h"
#include <cstddef>
#include <execution>
#include<utility>

static const size_t MAX_PARTICLES_PER_GENERATOR = 512;

ParticleGenerator new_particle_generator(ParticleSettings&& settings){
    size_t initialCapacity = std::max(32, settings.spawnQuantity.max);
    if(settings.lifetime.max >= settings.spawnPeriod.max){
        initialCapacity *= 2;
    }
    ParticleGenerator result = ParticleGenerator {
        .settings = std::move(settings),
        .particlePool = {},
        .particleCurrentSpawnTimer = 0.f
    };
    result.particlePool.reserve(initialCapacity);
    return result;
}

void particle_generator_enable(ParticleGenerator& particles){
    if(particles.particleCurrentSpawnTimer < 0.f){
        particles.particleCurrentSpawnTimer = 0.0f;
    }
}

void particle_generator_disable(ParticleGenerator& particles){
    if(particles.particleCurrentSpawnTimer > 0.f){
        particles.particleCurrentSpawnTimer = -1.f;
    }
}

void particle_generator_reset(ParticleGenerator& particles){
    particles.particlePool.clear();
    particles.particleCurrentSpawnTimer = 0.f;
}

size_t particle_generator_current_number_of_particles(const ParticleGenerator& particles){
    return particles.particlePool.size();
}

void particle_generator_draw(const ParticleGenerator& particles, const Position& pos){
    Texture texture = particles.settings.texture;
    Vector2 posVector = to_Vector2(pos);
    for(const Particle& particle : particles.particlePool){
        particle.draw(texture, posVector);
    }
}

static void particle_generator_erase_dead_particles(ParticleGenerator& particles){
    for(size_t i = 0; i < particles.particlePool.size(); /*noop*/){
        if(particles.particlePool[i].shouldDie()){
            std::swap<Particle>(particles.particlePool[i], particles.particlePool.back());
            particles.particlePool.pop_back();
        } else {
            i++;
        }
    }
}

static void particle_generator_spawn_particles(ParticleGenerator& particles, RNGComponent& rng){
    size_t numberToSpawn = particles.settings.spawnQuantity.get_random_fast(rng);
    ParticleSettings& settings = particles.settings;
    for(size_t i = 0; i < numberToSpawn; i++){
        Vector2 spawnPoint = Vector2 {
            settings.spawningArea.x + random_float(rng, settings.spawningArea.width),
            settings.spawningArea.y + random_float(rng, settings.spawningArea.height)
        };
        particles.particlePool.emplace_back(
            settings.color.get_random(rng),
            spawnPoint,
            settings.initialVelocity.get_random(rng),
            settings.acceleration,
            settings.initialRotation.get_random(rng),
            settings.rotationalVelocity.get_random(rng),
            settings.lifetime.get_random(rng)
        );
    }
}

void particle_generator_update(ParticleGenerator& particles, RNGComponent& rng, float delta){
    particles.particleCurrentSpawnTimer -= delta;
    if(particles.particleCurrentSpawnTimer <= 0.f){
        particle_generator_spawn_particles(particles, rng);
        particles.particleCurrentSpawnTimer = particles.settings.spawnPeriod.get_random(rng);
    }
    for(Particle& particle : particles.particlePool){
        particle.update(delta);
    }
    particle_generator_erase_dead_particles(particles);
}
