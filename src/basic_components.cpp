#include"basic_components.h"
#include<cmath>
#include"vector2_utilities.h"

Position::Position(float coord1, float coord2, CoordinateType type){
    if(type == CARTESIAN){
        x = coord1; y = coord2;
    } else if(type == POLAR){
        x = coord1*cos(coord2);
        y = coord1*sin(coord2);
    }
}

Velocity::Velocity(float coord1, float coord2, CoordinateType type){
    if(type == CARTESIAN){
        v_x = coord1; v_y = coord2;
    } else if(type == POLAR){
        v_x = coord1*cos(coord2);
        v_y = coord1*sin(coord2);
    }
}

Acceleration::Acceleration(float coord1, float coord2, CoordinateType type){
    if(type == CARTESIAN){
        a_x = coord1; a_y = coord2;
    } else if(type == POLAR){
        a_x = coord1*cos(coord2);
        a_y = coord1*sin(coord2);
    }
}

void move_position(Position& pos, const Velocity& vel, float delta){
    pos.x += vel.v_x * delta;
    pos.y += vel.v_y * delta;
}

void move_position(Position& pos, Velocity& vel, const Acceleration& acc, float delta){
    vel.v_x += acc.a_x * delta;
    vel.v_y += acc.a_y * delta;
    pos.x += vel.v_x * delta;
    pos.y += vel.v_y * delta;
}

SpriteSheet::SpriteSheet(const char *filename, unsigned int frameWidth, unsigned int frameHeight, const Vector2& offset = {0,0}): 
texture(LoadTexture(filename)), numberFramesPerRow(texture.width / frameWidth), numberRows(texture.height / frameHeight), 
numberFramesPerAnimation(numberRows), currentAnimation(0), currentFrame(0), offset(offset) {}

SpriteSheet::~SpriteSheet(){
    UnloadTexture(texture);
}

void SpriteSheet::set_animation_length(unsigned int animationRow, unsigned int length){
    numberFramesPerAnimation[animationRow] = length;
}

void next_frame(SpriteSheet& sprite){
    sprite.currentFrame = (sprite.currentFrame + 1) % sprite.numberFramesPerAnimation[sprite.currentAnimation];
}

void prev_frame(SpriteSheet& sprite){
    sprite.currentFrame--;
    if(sprite.currentFrame < 0) sprite.currentFrame = sprite.numberFramesPerAnimation[sprite.currentAnimation] - 1;
}

void draw_sprite(const SpriteSheet& sprite, const Position& pos){
    int frameWidth = sprite.texture.width / sprite.numberFramesPerRow;
    int frameHeight = sprite.texture.height / sprite.numberRows;
    Rectangle frame {frameWidth * sprite.currentFrame, frameHeight * sprite.currentAnimation, frameWidth, frameHeight};
    DrawTextureRec(sprite.texture, frame, sprite.offset + Vector2{pos.x, pos.y}, WHITE);
}
