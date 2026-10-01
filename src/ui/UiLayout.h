#pragma once

#include <algorithm>

#include "raylib.h"
#include "config/UiConfig.h"

namespace ui {

struct UiLayout {
    float scale;
    float offsetX;
    float offsetY;

    Rectangle Rect(Rectangle design) const {
        return {offsetX + design.x * scale, offsetY + design.y * scale,
                design.width * scale, design.height * scale};
    }

    Vector2 Point(float x, float y) const {
        return {offsetX + x * scale, offsetY + y * scale};
    }
};

inline UiLayout CurrentLayout() {
    const float scale = std::min(GetScreenWidth() / static_cast<float>(config::designWidth),
                                 GetScreenHeight() / static_cast<float>(config::designHeight));
    return {scale,
            (GetScreenWidth() - config::designWidth * scale) / 2.0f,
            (GetScreenHeight() - config::designHeight * scale) / 2.0f};
}

}  // namespace ui
