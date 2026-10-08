#include "ui/PlayerBackground.h"
#include <algorithm>
namespace ui {
namespace { Texture2D texture{}; }
bool LoadPlayerBackground(const char* path) {
    const auto candidate = LoadTexture(path);
    if (!IsTextureValid(candidate)) return false;
    SetTextureFilter(candidate, TEXTURE_FILTER_BILINEAR);
    UnloadPlayerBackground(); texture = candidate;
    return true;
}
void UnloadPlayerBackground() {
    if (IsTextureValid(texture)) UnloadTexture(texture);
    texture = {};
}
void DrawPlayerBackground(Rectangle bounds) {
    if (!IsTextureValid(texture)) return;
    const float scale = std::max(bounds.width / texture.width, bounds.height / texture.height);
    const float width = bounds.width / scale, height = bounds.height / scale;
    DrawTexturePro(texture, {(texture.width - width) / 2, (texture.height - height) / 2, width, height},
                   bounds, {0, 0}, 0, WHITE);
}
}
