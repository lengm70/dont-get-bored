#include "ui/BackgroundImage.h"
#include "ui/Motion.h"
#include "ui/AdaptiveTheme.h"

#include <algorithm>
#include <chrono>

#include "raylib.h"
#include "config/UiConfig.h"
#include "platform/MusicFileDialog.h"

namespace ui {
namespace {
Texture2D background{}, previous{};
float reveal=1;
void UseBackground(Texture2D candidate) {
    SetTextureFilter(candidate, TEXTURE_FILTER_POINT);
    if(IsTextureValid(previous)) UnloadTexture(previous);
    previous=background;
    reveal=IsTextureValid(previous)?0.f:1.f;
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
    if(IsTextureValid(previous)) UnloadTexture(previous);
    previous={}; background = {}; reveal=1;
}

void UpdateBackgroundAnimation(float seconds) {
    reveal=motion::Advance(reveal,animationConfig::backgroundDuration,seconds);
    if(reveal>=1 && IsTextureValid(previous)) {UnloadTexture(previous);previous={};}
}
void DrawBackgroundImage(int width, int height) {
    if (width <= 0 || height <= 0) return;
    if (!IsTextureValid(background)) {
        DrawRectangleGradientV(0, 0, width, height, config::gradientTop, config::gradientBottom);
        return;
    }
    const auto draw=[&](Texture2D texture,float opacity) {
        const float scale=std::max(width/static_cast<float>(texture.width),height/static_cast<float>(texture.height));
        const Rectangle destination{(width-texture.width*scale)/2,(height-texture.height*scale)/2,
            texture.width*scale,texture.height*scale};
        DrawTexturePro(texture,{0,0,static_cast<float>(texture.width),static_cast<float>(texture.height)},
            destination,{0,0},0,Fade(WHITE,opacity));
    };
    if(IsTextureValid(previous)) draw(previous,1);
    draw(background,motion::Ease(reveal));
}

}  // namespace ui
