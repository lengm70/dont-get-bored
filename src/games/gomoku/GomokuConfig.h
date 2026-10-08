#pragma once
#include "raylib.h"
#include "ui/UiPalette.h"
namespace games::gomoku::config {
inline constexpr float boardX = 298, boardY = 156, spacing = 26;
inline constexpr float stoneRadius = 11, markerRadius = 3;
inline constexpr Rectangle boardPanel{280, 138, 400, 400};
inline constexpr float titleY = 54, titleSize = 32, statusY = 104, statusSize = 20;
inline constexpr float controlsY = 552, controlsSize = 15;
inline constexpr float setupLabelX = 240, setupControlX = 450, setupControlWidth = 270;
inline constexpr float modeY = 190, strengthY = 270, controlHeight = 48;
inline constexpr float setupHintY = 350, rulesHintY = 390, hintSize = 16;
inline constexpr Rectangle startButton{360, 440, 240, 44}, backButton{400, 508, 160, 38};
inline constexpr Color black{20, 16, 32, 255}, white{241, 232, 255, 255};
inline constexpr Color grid{113, 86, 146, 255};
inline constexpr int circleSegments = 32;
}  // namespace games::gomoku::config
