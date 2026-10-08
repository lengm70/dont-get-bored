#pragma once

#include <atomic>
#include "games/gomoku/GomokuGame.h"

namespace games::gomoku {
enum class Strength { Easy, Normal, Hard };
struct SearchOptions { int depth; int candidates; int milliseconds; int nodeLimit; };
inline SearchOptions OptionsFor(Strength strength) {
    switch (strength) {
        case Strength::Easy: return {1, 8, 120, 2000};
        case Strength::Hard: return {5, 14, 1200, 40000};
        default: return {3, 12, 500, 16000};
    }
}
struct SearchResult { Move move{-1, -1}; int depth = 0; int nodes = 0; };
SearchResult FindMove(Board board, Stone side, SearchOptions options, const std::atomic<bool>& cancelled);
}  // namespace games::gomoku
