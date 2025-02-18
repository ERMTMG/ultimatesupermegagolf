#pragma once
#include"raylib.h"
#include"vector2_utilities.h"
#include"basic_components.h"
#include"bounding_box.h"

struct CameraView{
    Camera2D cam;

    // operator -> that just returns a pointer to the wrapped camera, for ease
    // of use (allows doing camera->target instead of camera.cam.target)
    inline Camera2D* operator->(){
        return &cam;
    }
    inline const Camera2D* operator->() const{
        return &cam;
    }
};

CameraView camera_centered_at(const Position& pos = {0,0});
void move_camera(CameraView& camera, const Vector2& offset);
void set_camera_center(CameraView& camera, const Position& pos);
    #define CAMERA_ZOOM_IN false
    #define CAMERA_ZOOM_OUT true
void zoom_camera(CameraView& camera, float zoomFactor, bool zoomMode = CAMERA_ZOOM_IN);
BoundingBoxComponent get_camera_bb(const CameraView& camera, bool includeMargin = true);
bool is_in_view(const CameraView& camera, const BoundingBoxComponent& bb, const Position& pos = {0,0});
