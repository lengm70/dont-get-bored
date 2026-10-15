#pragma once
#include <algorithm>
#include <array>
#include "raylib.h"
namespace ui {
// Clockwise screen-space outline with clipped corners; no raster assets needed.
inline void DrawGeometricPanel(Rectangle bounds, float cut, Color fill, Color border, float stroke) {
    cut = std::min(cut, std::min(bounds.width, bounds.height) / 2);
    const float x = bounds.x, y = bounds.y, right = x + bounds.width, bottom = y + bounds.height;
    const std::array<Vector2, 8> points{{{x + cut,y},{right-cut,y},{right,y+cut},{right,bottom-cut},
        {right-cut,bottom},{x+cut,bottom},{x,bottom-cut},{x,y+cut}}};
    const Vector2 center{x + bounds.width / 2, y + bounds.height / 2};
    for (std::size_t index=0; index<points.size(); ++index) {
        const auto next=(index+1)%points.size();
        DrawTriangle(center,points[next],points[index],fill);
        if (stroke > 0) DrawLineEx(points[index],points[next],stroke,border);
    }
}
}  // namespace ui
