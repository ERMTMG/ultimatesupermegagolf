/*
    FILE: rng_component.h
    Defines a component to handle any random output in the game
*/
#pragma once
#include<random>
//#include some more files maybe

/*
    Component that is in charge of generating the random numbers for basically anything.
    Includes a LCE for random ints and an std::uniform_real_distribution for random floats.
*/
struct RNGComponent{
    std::minstd_rand intGenerator;
    std::uniform_real_distribution<float> floatGenerator;
};

// Constructs and returns an RNG component with a random seed. The seed is 'randomized' through 
// memory allocation of an std::string and type punning, so it may be UB on some devices. works on my machine tho
RNGComponent new_rng_component();
// Safe version of the above function, just uses time(nullptr) to get a pseudo-random seed 
RNGComponent new_rng_component_safe();
// Creates an RNG component with an already known seed
RNGComponent new_rng_component(unsigned int seed);

// Gets a random integer with no restrictions
unsigned int random_int(RNGComponent& rng);

// Both these functions get a random integer in the range [0, upperBound), but the function not
// tagged as "_fast" performs checks to ensure the numbers are being picked from a range the size of
// which is a multiple of upperBound
unsigned int random_int(RNGComponent& rng, unsigned int upperBound);
unsigned int random_int_fast(RNGComponent& rng, unsigned int upperBound);

// Both these functions get a random integer in the range [lowerBound, upperBound). The difference
// between the "_fast" function and the other one is the same as on the above functions
unsigned int random_int(RNGComponent& rng, unsigned int lowerBound, unsigned int upperBound);
unsigned int random_int_fast(RNGComponent& rng, unsigned int lowerBound, unsigned int upperBound);

// Gets a random float in the range [0.0, 1.0]
float random_float(RNGComponent& rng);
// Gets a random float in the range [0.0, upperBound]
float random_float(RNGComponent& rng, float upperBound);
// Gets a random float in the range [lowerBound, upperBound]
float random_float(RNGComponent& rng, float lowerBound, float upperBound);



