#pragma once
#include <array>
#include "raylib.h"
#include "ui/UiPalette.h"
namespace games::fruit::config {
inline constexpr Rectangle panel{180, 30, 600, 540};
inline constexpr float boardX = 330, boardY = 166;
inline constexpr float titleY = 52, statsY = 92, controlsY = 542;
inline constexpr float titleSize = 32, statsSize = 18, hintSize = 14;
inline constexpr float previewY = 140, nextX = 700, nextY = 250;
inline constexpr float overlayY = 290, overlayHintY = 335;
inline constexpr float outlineWidth = 2, glowWidth = 2;
inline constexpr std::array<Color, 11> colors{{
    {98, 234, 242, 255}, {185, 144, 255, 255}, {107, 174, 255, 255},
    {255, 170, 94, 255}, {245, 222, 116, 255}, {255, 119, 165, 255},
    {144, 235, 174, 255}, {231, 173, 255, 255}, {142, 212, 255, 255},
    {230, 239, 156, 255}, {92, 255, 194, 255}
}};
inline constexpr int minSides = 5;
inline constexpr float shellRadius = 0.94f, innerRadius = 0.62f, coreRadius = 0.18f;
}  // namespace games::fruit::config
