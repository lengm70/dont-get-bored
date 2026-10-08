#include "config/HighScores.h"

#include <charconv>
#include <fstream>
#include <string>
#include <system_error>

namespace app {
namespace {

std::string Trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

bool ParseScore(const std::string& text, int& score) {
    int parsed = 0;
    const char* begin = text.data();
    const char* end = begin + text.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc{} || result.ptr != end || parsed < 0) return false;
    score = parsed;
    return true;
}

}  // namespace

bool LoadHighScores(HighScores& scores, const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) return false;

    HighScores loaded;
    int legacyMinesweeper = 0;
    bool hasEasyRecord = false;
    std::string line;
    while (std::getline(input, line)) {
        const auto separator = line.find('=');
        if (separator == std::string::npos) continue;
        const std::string key = Trim(line.substr(0, separator));
        const std::string value = Trim(line.substr(separator + 1));
        if (key == "snake") ParseScore(value, loaded.snake);
        else if (key == "tetris") ParseScore(value, loaded.tetris);
        else if (key == "breakout") ParseScore(value, loaded.breakout);
        else if (key == "minesweeper_best_ms") ParseScore(value, legacyMinesweeper);
        else if (key == "minesweeper_easy_ms") {
            hasEasyRecord |= ParseScore(value, loaded.minesweeperEasyMilliseconds);
        }
        else if (key == "minesweeper_normal_ms") ParseScore(value, loaded.minesweeperNormalMilliseconds);
        else if (key == "minesweeper_hard_ms") ParseScore(value, loaded.minesweeperHardMilliseconds);
    }
    if (input.bad()) return false;
    if (!hasEasyRecord) loaded.minesweeperEasyMilliseconds = legacyMinesweeper;
    scores = loaded;
    return true;
}

bool SaveHighScores(const HighScores& scores, const std::filesystem::path& path) {
    if (scores.snake < 0 || scores.tetris < 0 || scores.breakout < 0 ||
        scores.minesweeperEasyMilliseconds < 0 || scores.minesweeperNormalMilliseconds < 0 ||
        scores.minesweeperHardMilliseconds < 0) return false;
    std::error_code error;
    const auto parent = path.parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent, error);
    if (error) return false;

    std::ofstream output(path, std::ios::trunc);
    if (!output) return false;
    output << "# don't get bored high scores\n"
           << "snake=" << scores.snake << '\n'
           << "tetris=" << scores.tetris << '\n'
           << "breakout=" << scores.breakout << '\n'
           << "minesweeper_easy_ms=" << scores.minesweeperEasyMilliseconds << '\n'
           << "minesweeper_normal_ms=" << scores.minesweeperNormalMilliseconds << '\n'
           << "minesweeper_hard_ms=" << scores.minesweeperHardMilliseconds << '\n';
    output.close();
    return output.good();
}

int MinesweeperBest(const HighScores& scores, games::minesweeper::Difficulty level) {
    using games::minesweeper::Difficulty;
    switch (level) {
        case Difficulty::Easy: return scores.minesweeperEasyMilliseconds;
        case Difficulty::Normal: return scores.minesweeperNormalMilliseconds;
        case Difficulty::Hard: return scores.minesweeperHardMilliseconds;
        case Difficulty::Custom: return 0;
    }
    return 0;
}

bool RecordMinesweeperWin(HighScores& scores, games::minesweeper::Difficulty level,
                         int milliseconds) {
    using games::minesweeper::Difficulty;
    if (milliseconds <= 0) return false;
    int* record = nullptr;
    switch (level) {
        case Difficulty::Easy: record = &scores.minesweeperEasyMilliseconds; break;
        case Difficulty::Normal: record = &scores.minesweeperNormalMilliseconds; break;
        case Difficulty::Hard: record = &scores.minesweeperHardMilliseconds; break;
        case Difficulty::Custom: return false;
        default: return false;
    }
    if (*record != 0 && *record <= milliseconds) return false;
    *record = milliseconds;
    return true;
}

}  // namespace app
