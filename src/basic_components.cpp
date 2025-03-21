#include"basic_components.h"
#include<cmath>
#include"utility.h"

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

// TODO: This constructor loads the same image multiple times if it's called multiple times with the same filename.
// I assume that's pretty bad, so keep track of which textures have been loaded and avoid loading duplicates.
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
        .x = source.x + transform.offset.x - (source.width * (transform.scale.x - 1))/2,
        .y = source.y + transform.offset.y - (source.height * (transform.scale.y - 1))/2,
        .width = source.width * transform.scale.x,
        .height = source.height * transform.scale.y
    };
}

// TODO: this rotates the sprite around the top left point. Make it so that it rotatesaround its center
void draw_sprite(const SpriteSheet& sprite, const SpriteTransform& transform, const Position& pos){
    int frameWidth = sprite.texture.width / sprite.numberFramesPerRow;
    int frameHeight = sprite.texture.height / sprite.numberRows;
    Rectangle frame {frameWidth * sprite.currentFrame, frameHeight * sprite.currentAnimation, frameWidth, frameHeight};
    Rectangle destFrame = {pos.x - frameWidth/2.0f, pos.y - frameHeight/2.0f, frameWidth, frameHeight};
    destFrame = transform_frame_rect(destFrame, transform);
    destFrame.x += destFrame.width / 2; destFrame.y += destFrame.height / 2;
    DrawTexturePro(sprite.texture, frame, destFrame, {destFrame.width / 2, destFrame.height / 2}, transform.rotation, WHITE);
}

