#pragma once
#include"raylib.h"
#include"DS/heaparray.h"
#include<vector>

enum CoordinateType {
    CARTESIAN, POLAR
};

struct Position{
    float x;
    float y;

    // initializes based on a pair of coordinates. If type == CARTESIAN 
    // the coordinates are taken to be (x,y), and if TYPE == POLAR the
    // coordinates are taken to be (r, θ).
    Position(float coord1, float coord2, CoordinateType type = CARTESIAN);
};

struct Velocity{
    float v_x;
    float v_y;
    Velocity(float coord1, float coord2, CoordinateType type = CARTESIAN);
};

struct Acceleration{
    float a_x;
    float a_y;
    Acceleration(float coord1, float coord2, CoordinateType type = CARTESIAN);
};

struct SpriteSheet{
    Texture2D texture;
    unsigned short numberFramesPerRow;
    unsigned short numberRows; //AKA number of animations
    HeapArray<unsigned short> numberFramesPerAnimation;
    unsigned short currentAnimation;
    unsigned short currentFrame;

    SpriteSheet(const char* filename, unsigned int frameWidth, unsigned int frameHeight);
    ~SpriteSheet();
    void set_animation_length(unsigned int animationRow, unsigned int length);
};


void move_position(Position& pos, const Velocity& vel, float delta);
void move_position(Position& pos, Velocity& vel, const Acceleration& acc, float delta);
void next_frame(SpriteSheet& sprite);
void prev_frame(SpriteSheet& sprite);
void draw_sprite(const SpriteSheet& sprite, const Position& pos);