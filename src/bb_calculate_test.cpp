#include<raylib.h>
#include"utility.h"
#include"basic_components.h"
#include"bounding_box.h"
#include"collision_component.h"

const int SCREENWIDTH = 500;
const int SCREENHEIGHT = 500;
const int TRIANGLE_SIZE = 75;
const LayerType LAYER = 1;



int main(){

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Collision Test");
    SetTargetFPS(60);

    Position pos1 {0, 0};
    Position pos2 {250, 250};

    Vector2 point1 = {-TRIANGLE_SIZE, -TRIANGLE_SIZE};
    Vector2 point2 = {+TRIANGLE_SIZE, -TRIANGLE_SIZE + TRIANGLE_SIZE/2};
    Vector2 point3 = {             0, +TRIANGLE_SIZE};

    CollisionComponent collider;
    collider.add_line(point1, point2);
    collider.add_line(point2, point3);
    collider.add_line(point3, point1);
    add_to_layer(collider, LAYER);
    BoundingBoxComponent individualBBs[3];
    bool showIndividualBBs = false;
    while(!WindowShouldClose()){
        Vector2 mousePos = GetMousePosition();
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            point3 = mousePos;
            collider.shapes.clear();
            collider.add_line(point1, point2);
            collider.add_line(point2, point3);
            collider.add_line(point3, point1);
        }
        if(IsKeyPressed(KEY_SPACE)){
            showIndividualBBs = !showIndividualBBs;
        }
        BoundingBoxComponent bb = calculate_bb(collider);
        for(int i = 0; i < 3; i++){
            individualBBs[i] = calculate_bb(collider.shapes[i].get());
        }
        BeginDrawing();
            ClearBackground(BLACK);
            draw_collision_debug(collider, {250, 250});
            if(!showIndividualBBs){
                draw_bb_debug(bb, {250, 250});
            } else {
                DrawText("Showing individual bounding boxes", 10, 10, 15, RED);
                for(int i = 0; i < 3; i++){
                    draw_bb_debug(individualBBs[i], {250, 250});
                }
            }
        EndDrawing();
    }

}
