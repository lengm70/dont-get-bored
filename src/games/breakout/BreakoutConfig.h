#pragma once

#include <array>

#include "raylib.h"
#include "ui/UiPalette.h"
#include "games/breakout/BreakoutRules.h"

namespace games::breakout::config {

inline constexpr Rectangle gamePanel{120, 40, 720, 545};
inline constexpr float boardX = 160.0f;
inline constexpr float boardY = 150.0f;
inline constexpr int titleY = 74;
inline constexpr int titleSize = 32;
inline constexpr int statsY = 116;
inline constexpr int statsSize = 21;
inline constexpr int footerY = 549;
inline constexpr int footerSize = 17;
inline constexpr int overlayTitleY = 298;
inline constexpr int overlayHintY = 341;
inline constexpr int overlayTitleSize = 30;
inline constexpr int overlayHintSize = 19;
inline constexpr float borderWidth = 2.0f;
inline const Color& boardColor = ui::palette::board;
inline constexpr Color paddleColor{91, 226, 204, 255};
inline constexpr Color ballColor{238, 246, 255, 255};
inline const Color& overlayColor = ui::palette::overlay;
inline constexpr std::array<Color, rules::brickRows> brickColors{{
    {237, 111, 116, 255}, {239, 165, 94, 255}, {244, 205, 88, 255},
    {109, 204, 128, 255}, {104, 143, 224, 255}
}};

}  // namespace games::breakout::config
