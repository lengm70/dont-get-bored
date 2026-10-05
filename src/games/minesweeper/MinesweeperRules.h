#pragma once

namespace games::minesweeper::rules {

inline constexpr int columns = 9;
inline constexpr int rows = 9;
inline constexpr int mineCount = 10;
inline constexpr int cellCount = columns * rows;
inline constexpr int safeCellCount = cellCount - mineCount;
inline constexpr int safeStartRadius = 1;
inline constexpr int millisecondsPerSecond = 1000;

}  // namespace games::minesweeper::rules
