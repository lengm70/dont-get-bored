#pragma once

#include "raylib.h"
#include "config/AppSettings.h"
#include "games/breakout/BreakoutGame.h"

namespace games::breakout {

void UpdateFromInput(BreakoutGame& game, float elapsedSeconds);
void DrawScreen(const BreakoutGame& game, Font font, app::Language language,
                int bestScore);

}  // namespace games::breakout
