#include"basic_components.h"
#include<cmath>
#include"vector2_utilities.h"

Position::Position(float coord1, float coord2, CoordinateType type){
    if(type == COORDINATES_CARTESIAN){
        x = coord1; y = coord2;
    } else if(type == COORDINATES_POLAR){
        x = coord1*cos(coord2);
        y = coord1*sin(coord2);
    }
}

Velocity::Velocity(float coord1, float coord2, CoordinateType type){
    if(type == COORDINATES_CARTESIAN){
        v_x = coord1; v_y = coord2;
    } else if(type == COORDINATES_POLAR){
        v_x = coord1*cos(coord2);
        v_y = coord1*sin(coord2);
    }
}

Acceleration::Acceleration(float coord1, float coord2, CoordinateType type){
    if(type == COORDINATES_CARTESIAN){
        a_x = coord1; a_y = coord2;
    } else if(type == COORDINATES_POLAR){
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

SpriteSheet::SpriteSheet(const char *filename, unsigned int frameWidth, unsigned int frameHeight): 
texture(LoadTexture(filename)), numberFramesPerRow(texture.width / frameWidth), numberRows(texture.height / frameHeight), 
numberFramesPerAnimation(numberRows), currentAnimation(0), currentFrame(0) {}

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

Rectangle transform_frame_rect(const Rectangle& source, const SpriteTransform& transform){
    return Rectangle{
        .x = source.x + transform.offset.x,
        .y = source.y + transform.offset.y,
        .width = source.width * transform.scale.x,
        .height = source.height * transform.scale.y
    };
}

void draw_sprite(const SpriteSheet& sprite, const SpriteTransform& transform, const Position& pos){
    int frameWidth = sprite.texture.width / sprite.numberFramesPerRow;
    int frameHeight = sprite.texture.height / sprite.numberRows;
    Rectangle frame {frameWidth * sprite.currentFrame, frameHeight * sprite.currentAnimation, frameWidth, frameHeight};
    Rectangle destFrame = {pos.x - frameWidth/2.0f, pos.y - frameHeight/2.0f, frameWidth, frameHeight};
    destFrame = transform_frame_rect(destFrame, transform);
    DrawTexturePro(sprite.texture, frame, destFrame, to_Vector2(pos), transform.rotation, WHITE);
}

