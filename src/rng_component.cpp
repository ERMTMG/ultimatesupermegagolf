#include "rng_component.h"
#include<ctime>

RNGComponent new_rng_component(){
    std::string seedString = "hello world";
    unsigned int seed = *(unsigned int*)(&seedString);
    return new_rng_component(seed);
}

RNGComponent new_rng_component_safe(){
    unsigned int seed = time(nullptr);
    return new_rng_component(seed);
}

RNGComponent new_rng_component(unsigned int seed){
    return RNGComponent{
        .intGenerator = std::minstd_rand(seed),
        .floatGenerator = std::uniform_real_distribution<float>(0, 1)
    };
}

unsigned int random_int(RNGComponent& rng){
    return rng.intGenerator();
}

unsigned int random_int(RNGComponent& rng, unsigned int upperBound){
    unsigned int excess = (rng.intGenerator.max() + 1) % upperBound;
    unsigned int rerollLimit = rng.intGenerator.max() - excess;
    unsigned int output;
    do{
        output = rng.intGenerator();
    } while(output > rerollLimit);
    return output % upperBound;
}

unsigned int random_int_fast(RNGComponent& rng, unsigned int upperBound){
    return rng.intGenerator() % upperBound;
}

unsigned int random_int(RNGComponent& rng, unsigned int lowerBound, unsigned int upperBound){
    return lowerBound + random_int(rng, upperBound - lowerBound);
}

unsigned int random_int_fast(RNGComponent& rng, unsigned int lowerBound, unsigned int upperBound){
    return lowerBound + rng.intGenerator() % (upperBound - lowerBound);
}

float random_float(RNGComponent& rng){
    return rng.floatGenerator(rng.intGenerator);
}

float random_float(RNGComponent& rng, float upperBound){
    return rng.floatGenerator(rng.intGenerator) * upperBound;
}

float random_float(RNGComponent& rng, float lowerBound, float upperBound){
    return (upperBound - lowerBound) * rng.floatGenerator(rng.intGenerator) + lowerBound;
}