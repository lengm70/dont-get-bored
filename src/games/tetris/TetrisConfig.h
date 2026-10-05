#pragma once

#include <array>

#include "raylib.h"
#include "ui/UiPalette.h"

namespace games::tetris::config {

inline constexpr int boardX = 310;
inline constexpr int boardY = 115;
inline constexpr int cellSize = 21;
inline constexpr int sideX = 545;
inline constexpr int nextLabelY = 128;
inline constexpr int previewX = 553;
inline constexpr int previewY = 160;
inline constexpr int previewCellSize = 18;
inline constexpr int scoreLabelY = 250;
inline constexpr int bestLabelY = 320;
inline constexpr int linesLabelY = 390;
inline constexpr int levelLabelY = 454;
inline constexpr int statValueOffsetY = 23;
inline constexpr int statLabelSize = 18;
inline constexpr int statValueSize = 25;
inline constexpr int titleY = 73;
inline constexpr int titleSize = 32;
inline constexpr int footerFirstY = 546;
inline constexpr int footerSecondY = 568;
inline constexpr int footerSize = 16;
inline constexpr int overlayTitleY = 286;
inline constexpr int overlayTitleSize = 29;
inline constexpr int overlayHintY = 329;
inline constexpr int overlayHintSize = 17;
inline constexpr float cellInset = 1.0f;

inline constexpr Color boardColor = ui::palette::board;
inline constexpr Color gridColor = ui::palette::grid;
inline constexpr Color overlayColor = ui::palette::overlay;
inline constexpr std::array<Color, 7> pieceColors{{
    {88, 214, 231, 255},   // I
    {244, 205, 88, 255},   // O
    {176, 130, 224, 255},  // T
    {109, 204, 128, 255},  // S
    {237, 111, 116, 255},  // Z
    {104, 143, 224, 255},  // J
    {239, 165, 94, 255}    // L
}};

}  // namespace games::tetris::config
