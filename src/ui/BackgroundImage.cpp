#include "ui/BackgroundImage.h"
#include "ui/AdaptiveTheme.h"

#include <algorithm>
#include <chrono>

#include "raylib.h"
#include "config/UiConfig.h"
#include "platform/MusicFileDialog.h"

namespace ui {
namespace {
Texture2D background{};
void UseBackground(Texture2D candidate) {
    SetTextureFilter(candidate, TEXTURE_FILTER_POINT);
    UnloadBackgroundImage();
    background = candidate;
    UpdateThemeFromBackground(background);
}
}  // namespace

bool LoadBackgroundImage(const char* path) {
    const Texture2D candidate = LoadTexture(path);
    if (!IsTextureValid(candidate)) {
        TraceLog(LOG_WARNING, "Could not load background: %s", path);
        return false;
    }
    UseBackground(candidate);
    return true;
}

bool ImportBackgroundImage(const std::filesystem::path& source,
                           const std::filesystem::path& destination) {
    std::error_code error;
    std::filesystem::create_directories(destination.parent_path(), error);
    if (error) return false;
    // Stage beside the destination for an atomic replacement on the same volume.
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto staged = destination.parent_path() /
        ("import-" + std::to_string(stamp) + destination.extension().string());
    const bool copied = platform::CopyFileTo(source, staged);
    const Texture2D candidate = copied ? LoadTexture(staged.string().c_str()) : Texture2D{};
    const bool valid = IsTextureValid(candidate);
    const bool replaced = valid && platform::ReplaceFileWith(staged, destination);
    if (!replaced) {
        if (valid) UnloadTexture(candidate);
        std::filesystem::remove(staged, error);
        return false;
    }
    UseBackground(candidate);
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
