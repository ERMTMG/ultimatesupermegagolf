#include "camera_view.h"

CameraView camera_centered_at(const Position& pos){
    static const float DEFAULT_CAMERA_ZOOM = 1.0;
    return CameraView{ Camera2D{
        {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}, // cam.offset
        to_Vector2(pos),                                     // cam.target
        0.0, DEFAULT_CAMERA_ZOOM                             // cam.rotation, cam.zoom
    }};
}

void move_camera(CameraView& camera, const Vector2& offset){
    camera->target += offset;
}

void set_camera_center(CameraView& camera, const Position& pos){
    Vector2 center = to_Vector2(pos);
    BoundingBoxComponent cameraBB = get_camera_bb(camera, false);
    Vector2 halfSize = {cameraBB.width / 2, cameraBB.height / 2};
    camera->target = center - halfSize;
}

void zoom_camera(CameraView& camera, float zoomFactor, bool zoomMode = CAMERA_ZOOM_IN){
    if(zoomMode == CAMERA_ZOOM_IN){
        camera->zoom *= zoomFactor;
    } else if(zoomMode == CAMERA_ZOOM_OUT){
        camera->zoom /= zoomFactor;
    }
}

BoundingBoxComponent get_camera_bb(const CameraView& camera, bool includeMargin = true){
    static const Vector2 MARGIN = {10, 10};

    Vector2 topLeft = GetScreenToWorld2D(VEC2_ZERO, camera.cam);
    Vector2 bottomRight = GetScreenToWorld2D({GetScreenWidth(), GetScreenHeight()}, camera.cam);
    if(includeMargin){
        topLeft -= MARGIN;
        bottomRight += MARGIN;
    }
    return BoundingBoxComponent{
        topLeft,                   // offset
        (bottomRight - topLeft).x, // width
        (bottomRight - topLeft).y  // height
    };
}

bool is_in_view(const CameraView& camera, const BoundingBoxComponent& bb, const Position& pos){
    return overlapping_bb(bb, get_camera_bb(camera), pos);
}
