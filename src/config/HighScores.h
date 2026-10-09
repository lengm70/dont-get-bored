#pragma once

#include <filesystem>
#include "games/minesweeper/MinesweeperDifficulty.h"

namespace app {

struct HighScores {
    int snake = 0;
    int tetris = 0;
    int breakout = 0;
    int fruit = 0;
    int minesweeperEasyMilliseconds = 0;  // Zero means no completed game yet.
    int minesweeperNormalMilliseconds = 0;
    int minesweeperHardMilliseconds = 0;
};

inline constexpr const char* highScoresPath = "config/highscores.ini";

int MinesweeperBest(const HighScores& scores, games::minesweeper::Difficulty level);
bool RecordMinesweeperWin(HighScores& scores, games::minesweeper::Difficulty level,
                         int milliseconds);

bool LoadHighScores(HighScores& scores,
                    const std::filesystem::path& path = highScoresPath);
bool SaveHighScores(const HighScores& scores,
                    const std::filesystem::path& path = highScoresPath);

}  // namespace app
