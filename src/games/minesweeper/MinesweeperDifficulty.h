#pragma once

#include <algorithm>

namespace games::minesweeper {

enum class Difficulty { Easy, Normal, Hard, Custom };

struct BoardSettings {
    int columns = 9;
    int rows = 9;
    int mines = 10;
};

namespace difficulty {
inline constexpr BoardSettings easy{9, 9, 10};
inline constexpr BoardSettings normal{16, 16, 40};
inline constexpr BoardSettings hard{30, 16, 99};
inline constexpr int minDimension = 5;
inline constexpr int maxColumns = 40;
inline constexpr int maxRows = 30;
inline constexpr int safeStartCells = 9;

inline constexpr BoardSettings Preset(Difficulty level) {
    switch (level) {
        case Difficulty::Normal: return normal;
        case Difficulty::Hard: return hard;
        default: return easy;
    }
}

inline constexpr int MaximumMines(BoardSettings settings) {
    return settings.columns * settings.rows - safeStartCells;
}

inline constexpr bool IsValid(BoardSettings settings) {
    return settings.columns >= minDimension && settings.columns <= maxColumns &&
           settings.rows >= minDimension && settings.rows <= maxRows &&
           settings.mines >= 1 && settings.mines <= MaximumMines(settings);
}

inline BoardSettings ClampCustom(BoardSettings settings) {
    settings.columns = std::clamp(settings.columns, minDimension, maxColumns);
    settings.rows = std::clamp(settings.rows, minDimension, maxRows);
    settings.mines = std::clamp(settings.mines, 1, MaximumMines(settings));
    return settings;
}
}  // namespace difficulty

}  // namespace games::minesweeper
