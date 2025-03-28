#pragma once
#include<raylib.h>
#include"vector2_util.h"

inline void draw_arrow(const Vector2& start, const Vector2& end, Color color, int arrowTipLength = 2, int width = 1){
    Vector2 vec = end - start;
    Vector2 arrowTipUnit = unit_vector(Vector2{vec.y, -vec.x});
    DrawLineEx(start, end, width, color);
    Vector2 arrowTip = end + arrowTipLength * (arrowTipUnit - unit_vector(vec));
    DrawLineEx(end, arrowTip, width, color);
    arrowTip = end + arrowTipLength * (-arrowTipUnit - unit_vector(vec));
    DrawLineEx(end, arrowTip, width, color);
}