#include <iostream>
#include <format>
#include <sstream>
#include "raylib.h"
#include "vector3_utilities.h"
#include "3dgeometry.h"
#include "fstream"

const std::string FILENAME = "./resources/models/cube.usmg3dm";
const int SCREENWIDTH = 800;
const int SCREENHEIGHT = 600;
const float MODEL_SCALE = 50;
const float CENTER_X = SCREENWIDTH / 2.0;
const float CENTER_Y = SCREENHEIGHT / 2.0;

inline Vector2 screen_project_coords(const Vector3& pos){
    return {CENTER_X + (MODEL_SCALE*pos.x), CENTER_Y - (MODEL_SCALE*pos.z)};
}

int main() {
    Mesh3D mesh;
    double TIME = 0;
    mesh.read_from_file(FILENAME);
    for(auto itr = mesh.nc_begin(); itr != mesh.nc_end(); ++itr){
        itr->pos = rotate(itr->pos, 0.45, X_AXIS);
        itr->pos = rotate(itr->pos, M_PI_2, Z_AXIS);
    }
    mesh.print_data(std::cout);
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Hello world!");
    SetTargetFPS(100);
    while(!WindowShouldClose()){
        TIME += 0.016666;
        std::cout << GetFrameTime() << '\n';
        /*for(auto itr = mesh.nc_begin(); itr != mesh.nc_end(); ++itr){
            (*itr).pos = rotate((*itr).pos, 0.03*cos(M_SQRT1_2*TIME), Z_AXIS);
            (*itr).pos = rotate((*itr).pos, 0.1*cos(TIME), Y_AXIS);
        }*/
        for(auto itr = mesh.nc_begin(); itr != mesh.nc_end(); ++itr){
            itr->pos *= 1.0005;
            itr->pos = rotate(itr->pos,0.1, Z_AXIS);
        }
        BeginDrawing();
            ClearBackground(BLACK);
            DrawFPS(10,10);
            for(const Vertex& vtx : mesh){
                Vector2 screenPos = screen_project_coords(vtx.pos);
                DrawCircle(screenPos.x, screenPos.y, 1.5, RAYWHITE);
            }
            for(auto itr = mesh.begin_e(); itr != mesh.end_e(); ++itr){
                const Vertex& vtx1 = mesh.vertex((*itr).vtx1);
                const Vertex& vtx2 = mesh.vertex((*itr).vtx2);
                Vector2 screenPos1 = screen_project_coords(vtx1.pos);
                Vector2 screenPos2 = screen_project_coords(vtx2.pos);
                DrawLine(screenPos1.x, screenPos1.y, screenPos2.x, screenPos2.y, RAYWHITE);
            }
        EndDrawing();
    }
    

}