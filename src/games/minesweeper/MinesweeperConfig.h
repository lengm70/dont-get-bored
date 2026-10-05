#pragma once

#include <array>

#include "raylib.h"
#include "ui/UiPalette.h"

namespace games::minesweeper::config {

inline constexpr float boardX = 300.0f;
inline constexpr float boardY = 175.0f;
inline constexpr float cellSize = 40.0f;
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
inline constexpr int overlayTitleY = 311;
inline constexpr int overlayHintY = 354;
inline constexpr int overlayTitleSize = 30;
inline constexpr int overlayHintSize = 19;
inline constexpr Color hiddenColor = ui::palette::control;
inline constexpr Color revealedColor = ui::palette::board;
inline constexpr Color hoveredColor = ui::palette::focused;
inline constexpr Color flagColor{244, 205, 88, 255};
inline constexpr Color mineColor{237, 111, 116, 255};
inline constexpr Color overlayColor = ui::palette::overlay;
inline constexpr std::array<Color, 8> numberColors{{
    {104, 170, 244, 255}, {109, 204, 128, 255}, {237, 111, 116, 255},
    {176, 130, 224, 255}, {239, 165, 94, 255}, {88, 214, 231, 255},
    {238, 246, 255, 255}, {160, 183, 206, 255}
}};

}  // namespace games::minesweeper::config
