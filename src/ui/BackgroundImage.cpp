#include "ui/BackgroundImage.h"

#include <algorithm>

#include "raylib.h"
#include "config/UiConfig.h"

namespace ui {
namespace {
Texture2D background{};
}  // namespace

bool LoadBackgroundImage() {
    if (IsTextureValid(background)) return true;
    background = LoadTexture(config::backgroundPath);
    if (!IsTextureValid(background)) {
        TraceLog(LOG_WARNING, "Could not load background: %s", config::backgroundPath);
        return false;
    }
    SetTextureFilter(background, TEXTURE_FILTER_POINT);
    return true;
}

void UnloadBackgroundImage() {
    if (IsTextureValid(background)) UnloadTexture(background);
    background = {};
}

void DrawBackgroundImage(int width, int height) {
    if (width <= 0 || height <= 0) return;
    if (!IsTextureValid(background)) {
        DrawRectangleGradientV(0, 0, width, height, config::gradientTop, config::gradientBottom);
        return;
    }
    // Cover the window without stretching the artwork or adding empty margins.
    const float scale = std::max(width / static_cast<float>(background.width),
                                 height / static_cast<float>(background.height));
    const Rectangle destination{(width - background.width * scale) / 2,
                                (height - background.height * scale) / 2,
                                background.width * scale, background.height * scale};
    DrawTexturePro(background, {0, 0, static_cast<float>(background.width),
                               static_cast<float>(background.height)},
                   destination, {0, 0}, 0, WHITE);
}

}  // namespace ui
