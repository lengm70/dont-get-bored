#pragma once

#include <array>

#include "raylib.h"
#include "ui/UiPalette.h"

namespace games::minesweeper::config {

inline constexpr float boardX = 300.0f;
inline constexpr float boardY = 175.0f;
inline constexpr float cellSize = 40.0f;
inline constexpr Rectangle boardArea{80, 175, 800, 360};
inline constexpr Rectangle gamePanel{60, 40, 840, 555};
inline constexpr Rectangle setupSelector{300, 184, 360, 46};
inline constexpr float setupLabelX = 278;
inline constexpr float setupControlX = 455;
inline constexpr float setupRowsY = 262;
inline constexpr float setupRowGap = 50;
inline constexpr float setupControlWidth = 250;
inline constexpr float setupControlHeight = 40;
inline constexpr float setupLabelOffset = 10;
inline constexpr int setupHintSize = 16;
inline constexpr Rectangle setupStart{278, 477, 235, 44};
inline constexpr Rectangle setupBack{537, 477, 145, 44};
inline constexpr int setupTitleY = 96;
inline constexpr int setupSubtitleY = 145;
inline constexpr int setupSummaryY = 280;
inline constexpr int setupRecordY = 335;
inline constexpr int setupHintY = 428;
inline constexpr int setupTextSize = 20;
inline constexpr float cellInset = 1.0f;
inline constexpr float symbolStroke = 2.0f;
inline constexpr float symbolMargin = 11.0f;
inline constexpr float mineRadius = 7.0f;
inline constexpr int numberSize = 24;
inline constexpr int titleY = 74;
inline constexpr int titleSize = 32;
inline constexpr int statsY = 116;
inline constexpr int statsSize = 19;
inline constexpr int outcomeY = 145;
inline constexpr int outcomeSize = 20;
inline constexpr int footerFirstY = 548;
inline constexpr int footerSecondY = 575;
inline constexpr int footerSize = 16;
inline constexpr float overlayTitleOffset = -38;
inline constexpr float overlayHintOffset = 5;
inline constexpr int overlayTitleSize = 30;
inline constexpr int overlayHintSize = 19;
inline const Color& hiddenColor = ui::palette::control;
inline const Color& revealedColor = ui::palette::board;
inline const Color& hoveredColor = ui::palette::focused;
inline constexpr Color flagColor{244, 205, 88, 255};
inline constexpr Color mineColor{237, 111, 116, 255};
inline const Color& overlayColor = ui::palette::overlay;
inline constexpr std::array<Color, 8> numberColors{{
    {104, 170, 244, 255}, {109, 204, 128, 255}, {237, 111, 116, 255},
    {176, 130, 224, 255}, {239, 165, 94, 255}, {88, 214, 231, 255},
    {238, 246, 255, 255}, {160, 183, 206, 255}
}};

}  // namespace games::minesweeper::config
