#pragma once
#include<random>
//#include some more files maybe

struct RNGComponent{
    std::minstd_rand intGenerator;
    std::uniform_real_distribution<float> floatGenerator;
};

RNGComponent new_rng_component();
RNGComponent new_rng_component(unsigned int seed);
unsigned int random_int(RNGComponent& rng);
unsigned int random_int(RNGComponent& rng, unsigned int upperBound);
unsigned int random_int_fast(RNGComponent& rng, unsigned int upperBound);
unsigned int random_int(RNGComponent& rng, unsigned int lowerBound, unsigned int upperBound);
unsigned int random_int_fast(RNGComponent& rng, unsigned int lowerBound, unsigned int upperBound);
float random_float(RNGComponent& rng);
float random_float(RNGComponent& rng, float upperBound);
float random_float(RNGComponent& rng, float lowerBound, float upperBound);



