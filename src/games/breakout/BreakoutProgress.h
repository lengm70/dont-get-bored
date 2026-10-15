#pragma once
#include <array>
#include <filesystem>
#include "games/breakout/BreakoutGame.h"
namespace games::breakout {
struct Progress {
    int unlockedLevels = 1;
    int bestScore = 0;
    std::array<int, rules::levelCount> levelBest{};
    bool screenShake = true;
};
inline constexpr const char* progressPath = "config/breakout.ini";
bool LoadProgress(Progress& progress, const std::filesystem::path& path = progressPath);
bool SaveProgress(const Progress& progress, const std::filesystem::path& path = progressPath);
bool RecordProgress(Progress& progress, const BreakoutGame& game);
} // namespace games::breakout
