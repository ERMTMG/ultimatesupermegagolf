#include"raylib.h"
#include"entt.hpp"
#include"vector2_utilities.h"
#include"basic_components.h"
#include"level_registry.h"
#include<iostream>

static const int SCREENWIDTH = 800;
static const int SCREENHEIGHT = 600;

void add_walls(LevelRegistry& registry){
    auto[wallEntityID, walls] = registry.create_static_body(Position{0, 0}, {LevelRegistry::PLAYER_COLLISION_LAYER});
    walls.add_barrier({0,-120}, VEC2_DOWN_UNIT); // north wall
    walls.add_barrier({0,120}, VEC2_UP_UNIT); // south wall
    walls.add_barrier({-120,0}, VEC2_RIGHT_UNIT); // west wall
    walls.add_barrier({120,0}, VEC2_LEFT_UNIT); // east wall
    auto& bb = registry.get().emplace_or_replace<BoundingBoxComponent>(wallEntityID);
    bb = BoundingBoxComponent{{-150,-150},300,300};
    //walls.add_barrier(VEC2_ZERO, {-1,-1});
}

int main(){
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Ultimate Super Mega Golf");
    SetTargetFPS(60);

    LevelRegistry registry;
    registry.init_level(Position{-100, -100}, Position{100, 100}, Position{0,0});
    entt::entity cameraEntity = registry.get_entity(registry.CAMERA_ENTITY_NAME);
    CameraView& camera = registry.get().get<CameraView>(cameraEntity);
    add_walls(registry);
    auto[thing, thingCollision] = registry.create_static_body(Position{0,0}, {LevelRegistry::PLAYER_COLLISION_LAYER});
    thingCollision.add_line({-20,10},{20,10});
    thingCollision.add_line({20,10},{0,-15});
    thingCollision.add_line({0,-15},{-20,10});
    registry.recalculate_bounding_box(thing);

    while(!WindowShouldClose()){
        float delta = GetFrameTime();
        registry.update(delta);
        registry.draw(/*debug mode = */true);
        if(IsKeyDown(KEY_KP_ADD)){
            zoom_camera(camera, 1.01, CAMERA_ZOOM_IN);
        } else if(IsKeyDown(KEY_KP_SUBTRACT)){
            zoom_camera(camera, 1.01, CAMERA_ZOOM_OUT);
        }
        if(IsKeyDown(KEY_W)){
            move_camera(camera, {0, -10});
        }
        if(IsKeyDown(KEY_S)){
            move_camera(camera, {0, 10});
        }
        if(IsKeyDown(KEY_A)){
            move_camera(camera, {-10, 0});
        }
        if(IsKeyDown(KEY_D)){
            move_camera(camera, {10, 0});
        }
        //std::cout << "camera coordinates: " << GetScreenToWorld2D({0,0}, camera.cam) << " to " << GetScreenToWorld2D({SCREENWIDTH, SCREENHEIGHT}, camera.cam) << '\n';
        //std::cout << "\tplayer position: " << to_Vector2(registry.get().get<Position>(registry.get_entity(registry.PLAYER_ENTITY_NAME))) << '\n';
    }
}
