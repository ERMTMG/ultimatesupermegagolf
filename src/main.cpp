#include"raylib.h"
#include"entt.hpp"
#include"utility.h"
#include"basic_components.h"
#include"level_registry.h"
#include<iostream>

static const int SCREENWIDTH = 800;
static const int SCREENHEIGHT = 600;

void add_walls(LevelRegistry& registry){
    auto[wallEntityID, walls] = registry.create_static_body(Position{0, 0}, {LevelRegistry::PLAYER_COLLISION_LAYER});
    walls.add_barrier({0,-128}, VEC2_DOWN_UNIT); // north wall
    walls.add_barrier({0,128}, VEC2_UP_UNIT); // south wall
    walls.add_barrier({-128,0}, VEC2_RIGHT_UNIT); // west wall
    walls.add_barrier({128,0}, VEC2_LEFT_UNIT); // east wall
    auto& bb = registry.get().emplace_or_replace<BoundingBoxComponent>(wallEntityID);
    bb = BoundingBoxComponent{{-150,-150},300,300};
    //walls.add_barrier(VEC2_ZERO, {-1,-1});
}

void add_random_jolly(LevelRegistry& registry){
    entt::entity rngEntity = registry.get_entity(registry.RNG_ENTITY_NAME);
    RNGComponent& rng = registry.get().get<RNGComponent>(rngEntity);
    Vector2 pos = {random_float(rng, -80, 100), random_float(rng, -80, 100)};
    float radius = random_float(rng, 4, 16);
    auto[entity, collision] = registry.create_static_body(Position{pos}, {registry.PLAYER_COLLISION_LAYER});
    collision.add_circle(radius);
    registry.get().emplace<SpriteSheet>(entity, "resources/sprites/jolly_little_guy.png", 32, 32);
    registry.get().emplace<SpriteTransform>(entity, VEC2_ZERO, radius/16.0, 0);
    registry.get().emplace_or_replace<BoundingBoxComponent>(entity, calculate_bb(collision));
}

void add_square(LevelRegistry& registry, const Position& pos){
    auto[square, collision] = registry.create_static_body(pos, {LevelRegistry::PLAYER_COLLISION_LAYER});
    registry.get().emplace<SpriteSheet>(square, "resources/sprites/usmg_block.png", 32, 32);
    collision.add_rect_centered(32, 32);
    registry.recalculate_bounding_box(square);
}

void add_thing(LevelRegistry& registry, const Position& pos, int rotationDegrees){
    static const Vector2 POINT1 = {-20,10};
    static const Vector2 POINT2 = {20,10};
    static const Vector2 POINT3 = {0,-15};
    auto[thing, thingCollision] = registry.create_static_body(pos, {LevelRegistry::PLAYER_COLLISION_LAYER});
    thingCollision.add_line(rotate_degrees(POINT1, rotationDegrees), rotate_degrees(POINT2, rotationDegrees));
    thingCollision.add_line(rotate_degrees(POINT2, rotationDegrees), rotate_degrees(POINT3, rotationDegrees));
    thingCollision.add_line(rotate_degrees(POINT3, rotationDegrees), rotate_degrees(POINT1, rotationDegrees));
    registry.get().emplace<SpriteSheet>(thing, "resources/sprites/triangle_thing.png", 40, 25);
    registry.get().emplace<SpriteTransform>(thing, VEC2_ZERO, 1, rotationDegrees);
    BoundingBoxComponent bb = calculate_bb(thingCollision, 10.0);
    registry.get().emplace_or_replace<BoundingBoxComponent>(thing, bb);
}

int main(){
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Ultimate Super Mega Golf");
    SetTargetFPS(60);
    Image windowIcon = LoadImage("resources/sprites/ball_icon.png");
    SetWindowIcon(windowIcon); // This doesn't work because ubuntu uses GNOME for its desktop apparently. fuck
    UnloadImage(windowIcon);
    LevelRegistry registry;
    registry.init_level(Position{-100, -100}, Position{110, 100}, Position{0,0});
    entt::entity cameraEntity = registry.get_entity(registry.CAMERA_ENTITY_NAME);
    CameraView& camera = registry.get().get<CameraView>(cameraEntity);
    camera->zoom = 1.5;
    add_walls(registry);
    for(int i = -128; i < 128; i += 32){
        for(int j = -128; j < 128; j += 32){
            if((j == 64 || j == -64) && i <= 64){
                add_square(registry, {i+16,j+16});
            }
            if(j == 0 && i >= -64){
                add_square(registry, {i+16,j+16});
            }
        }
    }
    for(int i = -128; i <= 128; i+=40){
        add_thing(registry, {i+20,-128+12}, 23);
        add_thing(registry, {i+20,+128-12}, 536);
    }
    
    while(!WindowShouldClose()){
        float delta = GetFrameTime();
        registry.update(delta);
        registry.draw(/*debug mode = */false);
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
