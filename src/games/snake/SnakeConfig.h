#pragma once

#include "raylib.h"

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
inline constexpr float foodRadius = 7.0f;

inline constexpr Color boardColor{14, 28, 42, 255};
inline constexpr Color gridColor{42, 63, 77, 255};
inline constexpr Color headColor{91, 226, 204, 255};
inline constexpr Color bodyColor{53, 177, 151, 255};
inline constexpr Color foodColor{255, 138, 112, 255};
inline constexpr Color overlayColor{12, 24, 37, 220};

}  // namespace games::snake::config
