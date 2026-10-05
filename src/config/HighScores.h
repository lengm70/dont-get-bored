#pragma once

#include <filesystem>

namespace app {

struct HighScores {
    int snake = 0;
    int tetris = 0;
    int breakout = 0;
    int minesweeperBestMilliseconds = 0;  // Zero means no completed game yet.
};

inline constexpr const char* highScoresPath = "config/highscores.ini";

bool LoadHighScores(HighScores& scores,
                    const std::filesystem::path& path = highScoresPath);
bool SaveHighScores(const HighScores& scores,
                    const std::filesystem::path& path = highScoresPath);

}  // namespace app
