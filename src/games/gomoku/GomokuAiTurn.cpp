#include "games/gomoku/GomokuAiTurn.h"

namespace games::gomoku {
void GomokuAiTurn::Cancel() {
    cancelled_.store(true);
    if (worker_.joinable()) worker_.join();
    done_.store(false);
}

void GomokuAiTurn::Update(GomokuGame& game, Strength strength) {
    const bool computerTurn = game.State() == Status::Playing &&
        game.PlayMode() == Mode::Computer && game.Turn() == Stone::White;
    if (!computerTurn || (worker_.joinable() && game.MoveCount() != expectedMoves_)) {
        Cancel();
        return;
    }
    if (worker_.joinable()) {
        if (!done_.load(std::memory_order_acquire)) return;
        worker_.join();
        if (!cancelled_.load() && game.MoveCount() == expectedMoves_) game.Place(result_.move);
        return;
    }
    expectedMoves_ = game.MoveCount();
    cancelled_.store(false);
    done_.store(false);
    const Board snapshot = game.Cells();
    const SearchOptions options = OptionsFor(strength);
    worker_ = std::thread([this, snapshot, options] {
        result_ = FindMove(snapshot, Stone::White, options, cancelled_);
        done_.store(true, std::memory_order_release);
    });
}
}  // namespace games::gomoku
