#pragma once
#include"raylib.h"
#include"utility.h"
#include"sprite_loader.h"
#include<vector>

enum CoordinateType {
    COORDINATES_CARTESIAN, COORDINATES_POLAR
};

struct Position{
    float x;
    float y;

    // initializes based on a pair of coordinates. If type == CARTESIAN 
    // the coordinates are taken to be (x,y), and if TYPE == POLAR the
    // coordinates are taken to be (r, θ).
    Position(float coord1, float coord2, CoordinateType type = COORDINATES_CARTESIAN);
    Position(const Vector2& v) : Position(v.x, v.y) {};
};

struct Velocity{
    float v_x;
    float v_y;
    Velocity(float coord1, float coord2, CoordinateType type = COORDINATES_CARTESIAN);
    Velocity(const Vector2& v) : Velocity(v.x, v.y) {};
};

struct Acceleration{
    float a_x;
    float a_y;
    Acceleration(float coord1, float coord2, CoordinateType type = COORDINATES_CARTESIAN);
    Acceleration(const Vector2& v) : Acceleration(v.x, v.y) {};
};

//TODO: replace all hacky Vector2 <-> Position conversions with these functions and/or Position(Vector2) constructor.

inline Vector2 to_Vector2(const Position& pos){
    return {pos.x, pos.y};
}

inline Vector2 to_Vector2(const Velocity& vel){
    return {vel.v_x, vel.v_y};
}

inline Vector2 to_Vector2(const Acceleration& accel){
    return {accel.a_x, accel.a_y};
}

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

struct SpriteTransform{
    Vector2 offset;
    Vector2 scale;
    float rotation; // IN DEGREES!!!
    SpriteTransform() : offset{0,0}, rotation{0}, scale{1,1} {};
    SpriteTransform(const Vector2& offset, float scale, int rotation) : offset{offset}, rotation{rotation}, scale{scale, scale} {};
    SpriteTransform(const Vector2& offset, const Vector2& scale, int rotation) : offset{offset}, rotation{rotation}, scale{scale} {};
};

struct EntityName{
    std::string name;
};

void move_position(Position& pos, const Velocity& vel, float delta);
void move_position(Position& pos, Velocity& vel, const Acceleration& acc, float delta);
void next_frame(SpriteSheet& sprite);
void prev_frame(SpriteSheet& sprite);
Rectangle transform_frame_rect(const Rectangle& source, const SpriteTransform& transform);
void draw_sprite(const SpriteSheet& sprite, const SpriteTransform& transform, const Position& pos);
inline void draw_sprite(const SpriteSheet& sprite, const Position& pos){
    draw_sprite(sprite, SpriteTransform(), pos);
}