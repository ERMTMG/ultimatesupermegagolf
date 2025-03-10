#include"raylib.h"
#include"entt.hpp"
#include"vector2_utilities.h"
#include"basic_components.h"
#include"level_registry.h"

static const int SCREENWIDTH = 800;
static const int SCREENHEIGHT = 600;

void add_walls(LevelRegistry& registry){
    CollisionComponent& walls = registry.create_static_body(Position{0, 0}, {LevelRegistry::PLAYER_COLLISION_LAYER});
    walls.add_barrier({0,-200}, {0,1}); // north wall
    walls.add_barrier({0,200}, {0,-1}); // south wall
    walls.add_barrier({-200,0}, {1,0}); // west wall
    walls.add_barrier({200,0}, {-1,0}); // east wall
}

int main(){
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Ultimate Super Mega Golf");
    SetTargetFPS(60);

    LevelRegistry registry;
    registry.init_level(Position{-100, -100}, Position{100, 100}, Position{0,0});
    add_walls(registry);

    while(!WindowShouldClose()){
        float delta = GetFrameTime();
        registry.update(delta);
        registry.draw(true);
    }
}
