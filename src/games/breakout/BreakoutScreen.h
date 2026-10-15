#pragma once
#include "raylib.h"
#include "config/AppSettings.h"
#include "games/breakout/BreakoutGame.h"
#include "games/breakout/BreakoutProgress.h"
namespace games::breakout {
void UpdateFromInput(BreakoutGame& game, float elapsedSeconds, bool mouseCaptured = false,
                     const Progress* progress = nullptr);
void DrawScreen(const BreakoutGame& game, Font font, app::Language language,
                int bestScore, const Progress* progress = nullptr);
} // namespace games::breakout
