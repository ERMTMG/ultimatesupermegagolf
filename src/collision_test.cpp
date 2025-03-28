#include<raylib.h>
#include"utility.h"
#include"basic_components.h"
#include"collision_component.h"

const int SCREENWIDTH = 500;
const int SCREENHEIGHT = 500;
const int TRIANGLE_SIZE = 75;
const uint8_t LAYER = 1;

int main(){

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Collision Test");
    SetTargetFPS(60);

    Position pos1 {0, 0};
    Position pos2 {250, 250};

    CollisionComponent collider1(new CollisionCircle(VEC2_ZERO, 32));
    CollisionComponent collider2;
    /*collider2.add_line({-2*TRIANGLE_SIZE, TRIANGLE_SIZE}, {2*TRIANGLE_SIZE, TRIANGLE_SIZE});
    collider2.add_line({2*TRIANGLE_SIZE, TRIANGLE_SIZE},  {0, -2*TRIANGLE_SIZE});
    collider2.add_line({0, -2*TRIANGLE_SIZE}, {-2*TRIANGLE_SIZE, TRIANGLE_SIZE});*/
    collider2.add_rect_centered(100, 100);
    add_to_layer(collider1, LAYER); add_to_layer(collider2, LAYER);

    while(!WindowShouldClose()){
        CollisionInformation info = get_collision(collider1, collider2, pos1, pos2);
        pos1 = Position{GetMousePosition()};
        BeginDrawing();
            ClearBackground(BLACK);
            if(info.collision){
                DrawText("Objects colliding", 10, 10, 20, RED);
                draw_arrow(to_Vector2(pos1), to_Vector2(pos1) + info.unitNormal * 30, BLUE, 5);
            }
            draw_collision_debug(collider1, pos1);
            draw_collision_debug(collider2, pos2);
        EndDrawing();
    }

}
