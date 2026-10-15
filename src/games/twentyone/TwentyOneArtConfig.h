#pragma once
#include "raylib.h"
namespace games::twentyone::art {
inline constexpr float cardWidth = 160, cardHeight = 224, cornerCut = 12;
inline constexpr float cornerRankSize = 23, gridStep = 16;
inline constexpr Color warmSuit{255, 129, 173, 255}, coolSuit{116, 230, 255, 255};
inline constexpr float tableGrid = 40, gridOpacity = 0.10f, frameOpacity = 0.35f;
}  // namespace games::twentyone::art
