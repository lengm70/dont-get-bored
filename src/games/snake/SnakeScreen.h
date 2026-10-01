#pragma once

#include "raylib.h"
#include "config/AppSettings.h"
#include "games/snake/SnakeGame.h"

namespace games::snake {

void UpdateFromInput(SnakeGame& game, float elapsedSeconds);
void DrawScreen(const SnakeGame& game, Font font, app::Language language,
                int bestScore);

}  // namespace games::snake
