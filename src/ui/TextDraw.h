#pragma once

#include "raylib.h"
#include "config/UiConfig.h"
#include "ui/UiLayout.h"

namespace ui {

inline void DrawCenteredText(Font font, const char* text, float y, float size,
                             Color color, const UiLayout& layout) {
    const float fontSize = size * layout.scale;
    const float spacing = config::textSpacing * layout.scale;
    const Vector2 measured = MeasureTextEx(font, text, fontSize, spacing);
    DrawTextEx(font, text,
               Vector2{(GetScreenWidth() - measured.x) / 2.0f, layout.Point(0, y).y},
               fontSize, spacing, color);
}

inline void DrawScaledText(Font font, const char* text, float x, float y,
                           float size, Color color, const UiLayout& layout) {
    DrawTextEx(font, text, layout.Point(x, y), size * layout.scale,
               config::textSpacing * layout.scale, color);
}

}  // namespace ui
