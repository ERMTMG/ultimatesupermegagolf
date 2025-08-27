#include<raylib.h>
#include"entt.hpp"
#include"level_registry.h"
#include"level_builder.h"
#include<iostream>
#include<chrono>

const int SCREENWIDTH = 800;
const int SCREENHEIGHT = 600;
const char* const LEVEL_FILENAME = "resources/levels_json/test_level_colliders.json";
static bool DEBUG_MODE_ENABLED = false;

int main(int argc, char** argv){
    if(argc >= 2){
        std::string arg{argv[1]};
        if(arg.find("debug") != std::string::npos){
            DEBUG_MODE_ENABLED = true;
        }
    }
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Ultimate Super Mega Golf");
    SetTargetFPS(60);
    LevelRegistry level;
    {
        auto start = std::chrono::high_resolution_clock::now();

        LevelBuilder::Context context = LevelBuilder::init_level_parsing(LEVEL_FILENAME);
        if(context.error){
            std::cerr << "Error in context initialization, exiting...\n";
            exit(1);
        }
        LevelBuilder::build_level(context, level);

        auto end = std::chrono::high_resolution_clock::now();
        using milliseconds = std::chrono::duration<float, std::milli>;
        auto ms = std::chrono::duration_cast<milliseconds>(end - start);
        std::cout << "Level parsing complete, time taken: " << ms.count() << "ms\n";
        if(context.error){
            std::cerr << "Error in level building, exiting...\n";
            exit(2);
        }
    }

    while(!WindowShouldClose()){
        float delta = GetFrameTime();
        level.update(delta);
        level.draw(DEBUG_MODE_ENABLED);
        /*
        if(IsKeyDown(KEY_KP_ADD)){
            zoom_camera(camera, 1.01, CAMERA_ZOOM_IN);
        } else if(IsKeyDown(KEY_KP_SUBTRACT)){
            zoom_camera(camera, 1.01, CAMERA_ZOOM_OUT);
        }
        if(IsKeyDown(KEY_W)){
            move_camera(camera, {0, -1});
        }
        if(IsKeyDown(KEY_S)){
            move_camera(camera, {0, 1});
        }
        if(IsKeyDown(KEY_A)){
            move_camera(camera, {-1, 0});
        }
        if(IsKeyDown(KEY_D)){
            move_camera(camera, {1, 0});
        }
        */
        //std::cout << "camera coordinates: " << GetScreenToWorld2D({0,0}, camera.cam) << " to " << GetScreenToWorld2D({SCREENWIDTH, SCREENHEIGHT}, camera.cam) << '\n';
        //std::cout << "\tplayer position: " << to_Vector2(registry.get().get<Position>(registry.get_entity(registry.PLAYER_ENTITY_NAME))) << '\n';
    }

    level.get().clear();
}