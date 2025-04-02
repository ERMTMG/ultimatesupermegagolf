/*
    FILE: camera_view.h
    Defines the camera component and basic functions to manipulate the
    camera and do various checks with it.
*/
#pragma once
#include"raylib.h"
#include"utility.h"
#include"basic_components.h"
#include"bounding_box.h"

/*
    Represents a camera that views a part of the level and projects
    it to the screen. just a wrapper over Raylib's Camera2D struct.
    Potentially supports multiple cameras in one level, but might need
    some more coding (TODO later:)
*/
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
// Constructs a camera with the default zoom centered at the given position
CameraView camera_centered_at(const Position& pos = {0,0});
// Moves the camera at a relative offset
void move_camera(CameraView& camera, const Vector2& offset);
// Moves the camera so that the center is at the given position
void set_camera_center(CameraView& camera, const Position& pos);
    #define CAMERA_ZOOM_IN false
    #define CAMERA_ZOOM_OUT true
// Zooms the camera in or out by the given factor, which can be CAMERA_ZOOM_IN or CAMERA_ZOOM_OUT.
void zoom_camera(CameraView& camera, float zoomFactor, bool zoomMode = CAMERA_ZOOM_IN);
// Returns a bounding box that covers everything the camera sees. WARNING: Assumes the camera is NOT rotated!
BoundingBoxComponent get_camera_bb(const CameraView& camera, bool includeMargin = true);
// Checks if the given bounding box with the given offset is within the view of the camera (AKA, checks for 
// BB collision with the camera's bounding box)
bool is_in_view(const CameraView& camera, const BoundingBoxComponent& bb, const Position& pos = {0,0});
