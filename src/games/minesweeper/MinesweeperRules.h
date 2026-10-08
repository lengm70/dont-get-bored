#pragma once

#include "games/minesweeper/MinesweeperDifficulty.h"

namespace games::minesweeper::rules {

inline constexpr int columns = difficulty::easy.columns;
inline constexpr int rows = difficulty::easy.rows;
inline constexpr int mineCount = difficulty::easy.mines;
inline constexpr int cellCount = columns * rows;
inline constexpr int safeCellCount = cellCount - mineCount;
inline constexpr int safeStartRadius = 1;
inline constexpr int millisecondsPerSecond = 1000;

}  // namespace games::minesweeper::rules
