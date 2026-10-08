#pragma once

#include "raylib.h"
#include "ui/UiPalette.h"

namespace games::snake::config {

inline constexpr int boardX = 260;
inline constexpr int boardY = 170;
inline constexpr int cellSize = 22;
inline constexpr int titleY = 102;
inline constexpr int titleSize = 34;
inline constexpr int scoreY = 143;
inline constexpr int scoreSize = 21;
inline constexpr int footerY = 552;
inline constexpr int footerSize = 17;
inline constexpr int overlayTitleY = 291;
inline constexpr int overlayTitleSize = 34;
inline constexpr int overlayHintY = 339;
inline constexpr int overlayHintSize = 22;
inline constexpr float segmentInset = 1.5f;
inline constexpr float segmentRoundness = 0.25f;
inline constexpr int segmentSegments = 4;
inline constexpr float eyeRadius = 1.7f;
inline constexpr float eyeForwardOffset = 4.0f;
inline constexpr float eyeSideOffset = 4.0f;
inline constexpr float foodRadius = 7.0f;

inline const Color& boardColor = ui::palette::board;
inline const Color& gridColor = ui::palette::grid;
inline constexpr Color headColor{91, 226, 204, 255};
inline constexpr Color tailColor{35, 95, 88, 255};
inline constexpr Color eyeColor{12, 31, 40, 255};
inline constexpr Color foodColor{255, 138, 112, 255};
inline const Color& overlayColor = ui::palette::overlay;

}  // namespace games::snake::config
