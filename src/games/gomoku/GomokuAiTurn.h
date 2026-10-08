#pragma once

#include <atomic>
#include <thread>
#include "games/gomoku/GomokuAI.h"

namespace games::gomoku {
// Only Update touches the game. The worker owns a board snapshot.
class GomokuAiTurn {
public:
    ~GomokuAiTurn() { Cancel(); }
    void Update(GomokuGame& game, Strength strength);
    void Cancel();
    bool Thinking() const { return worker_.joinable(); }
private:
    std::thread worker_;
    std::atomic<bool> cancelled_{false};
    std::atomic<bool> done_{false};
    SearchResult result_{};
    int expectedMoves_ = 0;
};
}  // namespace games::gomoku
