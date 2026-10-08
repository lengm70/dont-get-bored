#include "ui/AdaptiveTheme.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace ui {
namespace {
constexpr int sampleSize = 64;
constexpr int hueBins = 24;
constexpr float minSaturation = 0.20f;
constexpr float minValue = 0.12f;
constexpr float minColorCoverage = 0.03f;
struct Hsv { float hue, saturation, value; };
Hsv ToHsv(Color color) {
    const float r = color.r / 255.0f, g = color.g / 255.0f, b = color.b / 255.0f;
    const float high = std::max({r, g, b}), low = std::min({r, g, b});
    const float delta = high - low;
    float hue = 0;
    if (delta > 0) {
        if (high == r) hue = 60 * std::fmod((g - b) / delta, 6.0f);
        else if (high == g) hue = 60 * ((b - r) / delta + 2);
        else hue = 60 * ((r - g) / delta + 4);
    }
    if (hue < 0) hue += 360;
    return {hue, high > 0 ? delta / high : 0, high};
}
Color FromHsv(float hue, float saturation, float value, unsigned char alpha = 255) {
    const float chroma = value * saturation;
    const float x = chroma * (1 - std::abs(std::fmod(hue / 60, 2.0f) - 1));
    const float offset = value - chroma;
    float r = 0, g = 0, b = 0;
    if (hue < 60) { r = chroma; g = x; }
    else if (hue < 120) { r = x; g = chroma; }
    else if (hue < 180) { g = chroma; b = x; }
    else if (hue < 240) { g = x; b = chroma; }
    else if (hue < 300) { r = x; b = chroma; }
    else { r = chroma; b = x; }
    return {static_cast<unsigned char>(std::lround((r + offset) * 255)),
            static_cast<unsigned char>(std::lround((g + offset) * 255)),
            static_cast<unsigned char>(std::lround((b + offset) * 255)), alpha};
}
struct Bucket { float weight = 0, r = 0, g = 0, b = 0; };
}  // namespace

palette::Colors ExtractTheme(const Color* pixels, std::size_t count) {
    std::array<Bucket, hueBins> buckets{};
    float visible = 0, colorful = 0;
    if (pixels) for (std::size_t index = 0; index < count; ++index) {
        const Color pixel = pixels[index];
        const float alpha = pixel.a / 255.0f;
        visible += alpha;
        const auto hsv = ToHsv(pixel);
        if (alpha < 0.25f || hsv.saturation < minSaturation || hsv.value < minValue) continue;
        colorful += alpha;
        const float weight = alpha * (0.5f + hsv.saturation);
        auto& bucket = buckets[std::min(hueBins - 1, static_cast<int>(hsv.hue * hueBins / 360))];
        bucket.weight += weight;
        bucket.r += pixel.r * weight; bucket.g += pixel.g * weight; bucket.b += pixel.b * weight;
    }
    float hue = 0, saturation = 0;
    if (colorful > 0 && colorful >= visible * minColorCoverage) {
        const auto best = std::max_element(buckets.begin(), buckets.end(),
            [](const Bucket& a, const Bucket& b) { return a.weight < b.weight; });
        const Color dominant{static_cast<unsigned char>(best->r / best->weight),
                             static_cast<unsigned char>(best->g / best->weight),
                             static_cast<unsigned char>(best->b / best->weight), 255};
        const auto hsv = ToHsv(dominant);
        hue = hsv.hue; saturation = std::clamp(hsv.saturation, 0.30f, 0.70f);
    }
    palette::Colors result;
    // Keep surfaces dark and text bright regardless of source brightness.
    const auto surface = [&](float value, unsigned char alpha = 255) {
        return FromHsv(hue, saturation * 0.65f, value, alpha);
    };
    result.text = FromHsv(hue, saturation * 0.06f, 0.98f);
    result.muted = FromHsv(hue, saturation * 0.18f, 0.78f);
    result.accent = FromHsv(hue, saturation * 0.70f, 0.98f);
    result.gradientTop = surface(0.07f); result.gradientBottom = surface(0.14f);
    result.panel = surface(0.14f, 250); result.board = surface(0.07f);
    result.control = surface(0.21f); result.focused = surface(0.28f); result.pressed = surface(0.34f);
    result.border = surface(0.52f); result.cardBorder = surface(0.42f);
    result.card = surface(0.18f); result.cardFocused = surface(0.24f); result.cardPressed = surface(0.30f);
    result.grid = surface(0.21f); result.shadow = surface(0.03f, 110); result.overlay = surface(0.05f, 240);
    return result;
}

void UpdateThemeFromBackground(Texture2D texture) {
    Image image = LoadImageFromTexture(texture);
    palette::Colors colors{};  // Readback failure uses the default violet palette.
    if (IsImageValid(image)) {
        ImageResize(&image, sampleSize, sampleSize);
        Color* pixels = LoadImageColors(image);
        if (pixels) colors = ExtractTheme(pixels, sampleSize * sampleSize);
        UnloadImageColors(pixels);
        UnloadImage(image);
    }
    palette::current = colors;
    ++palette::revision;
}
}  // namespace ui
