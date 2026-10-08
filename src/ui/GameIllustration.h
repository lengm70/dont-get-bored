#pragma once

#include "raylib.h"

namespace ui {

enum class GameIllustration { Snake, Tetris, Breakout, Minesweeper, Gomoku };

void DrawGameIllustration(GameIllustration game, Rectangle bounds, Font font);

}  // namespace ui
