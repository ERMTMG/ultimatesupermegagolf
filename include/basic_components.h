/*
    FILE: basic_components.h 
    Defines all of the most basic components that most game entities will have:
    Position, Velocity, Acceleration, SpriteSheet, SpriteTransform, EntityName,...
*/

#pragma once
#include"raylib.h"
#include"utility.h"
#include"sprite_loader.h"
#include<vector>

enum CoordinateType {
    COORDINATES_CARTESIAN, COORDINATES_POLAR
};

/*
 == [Common for Position, Velocity, Acceleration] == 
 Contains:
    - An x and y component
    - Conversion function to and from Raylib's Vector2 (to_Vector2 and 
      constructor (const Vector2&) respectively)
    - Constructor from a pair of coordinates that can be taken as cartesian 
      (x,y) or polar (r,θ). Example: 

        Position pos(2, M_PI_4, COORDINATES_POLAR); // Constructs Position(2/sqrt2, 2/sqrt2).
        Velocity vel(3, 5, COORDINATES_CARTESIAN); // Constructs Velocity(3,5).
                                                   // `Velocity vel(3,5);` would have the same result.
*/

struct Position{
    float x;
    float y;

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

inline Vector2 to_Vector2(const Position& pos){
    return {pos.x, pos.y};
}

inline Vector2 to_Vector2(const Velocity& vel){
    return {vel.v_x, vel.v_y};
}

inline Vector2 to_Vector2(const Acceleration& accel){
    return {accel.a_x, accel.a_y};
}

/*
    Defines a spritesheet with the possibility of multiple animations with 
    distinct frames, as long as all the frames have the same size.
    Wraps Raylib's Texture2D class. Uses the sprite loader functionality to 
    avoid loading duplicate textures.
*/
struct SpriteSheet{
    Texture2D texture; // Raylib texture to use
    unsigned short numberFramesPerRow; // Maximum number of frames per animation. Equivalent to texture.width/[frame width]
    unsigned short numberRows; // Total number of animations. Equivalent to texture.height/[frame height]
    /*
        Array that stores the number of frames for each individual animation.
        For animation i, numberFramesPerAnimation[i] returns its own number of
        frames.
        TODO later: is there a better way for this?
    */
    util::HeapArray<unsigned short> numberFramesPerAnimation; 
    unsigned short currentAnimation;
    unsigned short currentFrame;

    SpriteSheet(const char* filename, unsigned int frameWidth, unsigned int frameHeight);
    ~SpriteSheet();
    // equivalent to calling mySpriteSheet.numberFramesPerAnimation[animationRow] = length;
    void set_animation_length(unsigned int animationRow, unsigned short length);
};

/*
    Defines a transformation applied in the drawing of a sprite to the screen. 
    In case of an entity having a SpriteSheet but not a SpriteTransform, the
    sprite will be drawn with a default transform (offset taken to be VEC2_ZERO,
    scale taken to be VEC2_ONE and rotation taken to be zero). 
*/
struct SpriteTransform{
    Vector2 offset;
    Vector2 scale; // allows for defining a separate scale in x and y (squish/stretch)
    float rotation; // IN DEGREES!!!
    SpriteTransform() : offset{VEC2_ZERO}, rotation{0}, scale{VEC2_ONE} {};
    SpriteTransform(const Vector2& offset, float scale, float rotation) : offset{offset}, rotation{rotation}, scale{scale, scale} {};
    SpriteTransform(const Vector2& offset, const Vector2& scale, float rotation) : offset{offset}, rotation{rotation}, scale{scale} {};
};

/*
    Defines a 'name' for the entity. Not used yet, might be deleted.
*/
struct EntityName{
    std::string name;
};

// Moves a position according to a constant velocity with the specified delta-time
void move_position(Position& pos, const Velocity& vel, float delta);
// Moves a position according to a velocity which itself gets moved according to
// a constant acceleration, both with the specified delta-time
void move_position(Position& pos, Velocity& vel, const Acceleration& acc, float delta);
// Switches the given SpriteSheet to the next frame, looping back to the start when the
// animation's end is reached.
void next_frame(SpriteSheet& sprite);
// Switches the given SpriteSheet to the previous frame, looping back to the end when
// the animation's start is surpassed.
void prev_frame(SpriteSheet& sprite);
// [Auxiliary] Outputs the Rectangle resulting from applying the given SpriteTransform
// to the given rectangle which would correspond to the drawing rectangle of a SpriteSheet frame.
Rectangle transform_frame_rect(const Rectangle& source, const SpriteTransform& transform);

// Draws a sprite to the screen, with or without a SpriteTransform.
// Made to be used within Raylib's BeginDrawing() ... EndDrawing() functions.
void draw_sprite(const SpriteSheet& sprite, const SpriteTransform& transform, const Position& pos);
inline void draw_sprite(const SpriteSheet& sprite, const Position& pos){
    draw_sprite(sprite, SpriteTransform(), pos);
}