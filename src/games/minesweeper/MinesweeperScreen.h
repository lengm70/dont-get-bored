#pragma once

#include "raylib.h"
#include "config/AppSettings.h"
#include "games/minesweeper/MinesweeperGame.h"

namespace games::minesweeper {

void UpdateFromInput(MinesweeperGame& game, float elapsedSeconds, bool mouseCaptured = false);
void DrawScreen(const MinesweeperGame& game, Font font, app::Language language,
                int bestMilliseconds);

}  // namespace games::minesweeper
