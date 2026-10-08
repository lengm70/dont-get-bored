#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "ui/AdaptiveTheme.h"

namespace {
void Require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
float Luminance(Color color) {
    const auto linear = [](unsigned char channel) {
        const float value = channel / 255.0f;
        return value <= 0.04045f ? value / 12.92f : std::pow((value + 0.055f) / 1.055f, 2.4f);
    };
    return 0.2126f * linear(color.r) + 0.7152f * linear(color.g) + 0.0722f * linear(color.b);
}
float Contrast(Color a, Color b) {
    const float x = Luminance(a), y = Luminance(b);
    return (std::max(x, y) + 0.05f) / (std::min(x, y) + 0.05f);
}
}
int main() {
    try {
        std::vector<Color> pixels(900, {30, 90, 230, 255});
        pixels.resize(1000, {240, 20, 30, 255});
        const auto blue = ui::ExtractTheme(pixels.data(), pixels.size());
        Require(blue.accent.b > blue.accent.r && blue.accent.b > blue.accent.g,
                "Dominant blue region wins over small red region");
        pixels.assign(1000, {250, 30, 20, 0});
        pixels[0] = {25, 200, 60, 255};
        const auto green = ui::ExtractTheme(pixels.data(), pixels.size());
        Require(green.accent.g > green.accent.r && green.accent.g > green.accent.b,
                "Invisible RGB does not affect theme");
        for (Color neutral : {BLACK, WHITE, GRAY, Color{200, 10, 10, 0}}) {
            pixels.assign(1000, neutral);
            const auto theme = ui::ExtractTheme(pixels.data(), pixels.size());
            Require(theme.accent.r == theme.accent.g && theme.accent.g == theme.accent.b,
                    "Monochrome and transparent backgrounds use neutral palette");
        }
        for (int hue = 0; hue < 360; hue += 15) {
            pixels.assign(1000, ColorFromHSV(static_cast<float>(hue), 0.9f, 1));
            const auto theme = ui::ExtractTheme(pixels.data(), pixels.size());
            for (Color surface : {theme.panel, theme.control, theme.focused, theme.pressed,
                                  theme.card, theme.cardFocused, theme.cardPressed, theme.board}) {
                Require(Contrast(theme.text, surface) >= 4.5f, "Text contrast across all hues");
            }
            Require(Contrast(theme.muted, theme.panel) >= 4.5f, "Secondary text remains readable");
            Require(Contrast(theme.accent, theme.control) >= 3, "Accent remains visible");
            Require(theme.panel.a >= 250, "Bright backgrounds cannot wash out panel text");
        }
        const auto empty = ui::ExtractTheme(nullptr, 0);
        Require(empty.accent.r == empty.accent.g, "Empty analysis is safe");
        std::cout << "Adaptive theme tests passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n'; return 1;
    }
}
