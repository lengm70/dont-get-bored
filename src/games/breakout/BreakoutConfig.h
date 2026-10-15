#pragma once

#include <array>

#include "raylib.h"
#include "ui/UiPalette.h"
#include "games/breakout/BreakoutRules.h"

namespace games::breakout::config {

inline constexpr Rectangle gamePanel{120, 40, 720, 545};
inline constexpr float boardX = 160.0f;
inline constexpr float boardY = 150.0f;
inline constexpr int titleY = 58;
inline constexpr int titleSize = 32;
inline constexpr int statsY = 96;
inline constexpr int statsSize = 21;
inline constexpr float selectionX = 260, selectionY = 397;
inline constexpr float selectionWidth = 80, selectionHeight = 36, selectionGap = 90;
inline constexpr int secondaryHudY = 123, secondarySize = 16;
inline constexpr int overlayTitleY = 225, overlayHintY = 267;
inline constexpr int overlayTitleSize = 30, overlayHintSize = 19;
inline constexpr int summaryScoreY = 307, summaryComboY = 345, summaryBonusY = 380, summaryBestY = 416;
inline constexpr int summaryScoreSize = 22, summaryComboSize = 19, summaryBonusSize = 20, summaryBestSize = 17;
inline constexpr int levelNameY = 306, levelNameSize = 21, legendY = 344, legendSize = 13;
inline constexpr int selectionHintY = 449, selectionHintSize = 16, selectionFontSize = 18;
inline constexpr int footerY = 529, footerSize = 15, secondaryFooterY = 554, secondaryFooterSize = 14;
inline constexpr float gridSpacing = 40, flashSeconds = 0.12f, shakeAmplitude = 2.5f, shakeSeconds = 0.2f;
inline constexpr int fragmentCount = 8;
inline constexpr Color armorColor{151, 143, 242, 255};
inline constexpr Color explosionColor{255, 117, 141, 255};
inline constexpr Color obstacleColor{98, 105, 130, 255};
inline constexpr Color shieldColor{102, 172, 255, 255};
inline constexpr Color multiColor{220, 150, 255, 255};
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
