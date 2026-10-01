#pragma once

#include "raylib.h"
#include "config/AppSettings.h"
#include "games/tetris/TetrisGame.h"

namespace games::tetris {

void UpdateFromInput(TetrisGame& game, float elapsedSeconds);
void DrawScreen(const TetrisGame& game, Font font, app::Language language,
                int bestScore);

}  // namespace games::tetris
