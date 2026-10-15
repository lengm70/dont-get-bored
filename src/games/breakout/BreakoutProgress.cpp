#include "games/breakout/BreakoutProgress.h"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <string>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
namespace games::breakout {
namespace {
bool Valid(const Progress& progress) {
    return progress.unlockedLevels >= 1 && progress.unlockedLevels <= rules::levelCount &&
        progress.bestScore >= 0 && std::all_of(progress.levelBest.begin(), progress.levelBest.end(),
                                            [](int score) { return score >= 0; });
}
}
bool LoadProgress(Progress& progress, const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) return false;
    Progress loaded;
    std::string line;
    while (std::getline(input, line)) {
        const auto split = line.find('=');
        if (split == std::string::npos || line.empty() || line[0] == '#') continue;
        std::string key = line.substr(0, split), text = line.substr(split + 1);
        if (!text.empty() && text.back() == '\r') text.pop_back();
        int value = 0;
        auto result = std::from_chars(text.data(), text.data() + text.size(), value);
        if (result.ec != std::errc{} || result.ptr != text.data() + text.size() || value < 0) continue;
        if (key == "unlocked_levels" && value >= 1 && value <= rules::levelCount) loaded.unlockedLevels = value;
        else if (key == "best_score") loaded.bestScore = value;
        else if (key == "screen_shake" && value <= 1) loaded.screenShake = value == 1;
        else for (int i = 0; i < rules::levelCount; ++i)
            if (key == "level_" + std::to_string(i + 1) + "_best") loaded.levelBest[i] = value;
    }
    if (input.bad()) return false;
    progress = loaded;
    return true;
}
bool SaveProgress(const Progress& progress, const std::filesystem::path& path) {
    if (!Valid(progress)) return false;
    std::error_code error;
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path(), error);
    if (error) return false;
    auto temporary = path; temporary += ".tmp";
    std::ofstream output(temporary, std::ios::trunc);
    if (!output) return false;
    output << "# Breakout campaign progress\n"
           << "unlocked_levels=" << progress.unlockedLevels << '\n'
           << "best_score=" << progress.bestScore << '\n'
           << "screen_shake=" << (progress.screenShake ? 1 : 0) << '\n';
    for (int i = 0; i < rules::levelCount; ++i)
        output << "level_" << i + 1 << "_best=" << progress.levelBest[i] << '\n';
    output.close();
    if (!output.good()) { std::filesystem::remove(temporary, error); return false; }
#ifdef _WIN32
    const bool replaced = MoveFileExW(temporary.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    std::filesystem::rename(temporary, path, error);
    const bool replaced = !error;
#endif
    if (!replaced) std::filesystem::remove(temporary, error);
    return replaced;
}
bool RecordProgress(Progress& progress, const BreakoutGame& game) {
    const bool complete = game.State() == Status::LevelComplete || game.State() == Status::Won;
    if (!complete && game.State() != Status::GameOver) return false;
    const Progress previous = progress;
    progress.bestScore = std::max(progress.bestScore, game.Score());
    if (complete) {
        progress.levelBest[game.Level()] = std::max(progress.levelBest[game.Level()], game.LevelScore());
        progress.unlockedLevels = std::max(progress.unlockedLevels, std::min(rules::levelCount, game.Level() + 2));
    }
    return previous.bestScore != progress.bestScore || previous.unlockedLevels != progress.unlockedLevels ||
           previous.levelBest != progress.levelBest;
}
} // namespace games::breakout
