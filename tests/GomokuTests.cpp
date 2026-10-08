#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>
#include "games/gomoku/GomokuAiTurn.h"

namespace {
using namespace games::gomoku;
void Require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
void TestRules() {
    GomokuGame game;
    Require(!game.Place({7, 7}), "Ready board rejects moves");
    game.Start();
    Require(!game.Place({-1, 0}) && !game.Place({15, 0}), "Out of bounds moves rejected");
    Require(game.Place({7, 7}) && !game.Place({7, 7}), "Occupied cells rejected");
    Require(game.Turn() == Stone::White && game.MoveCount() == 1, "Invalid move preserves turn");
    for (Move direction : {Move{1, 0}, Move{0, 1}, Move{1, 1}, Move{1, -1}}) {
        game.Reset(); game.Start();
        for (int index = 0; index < 5; ++index) {
            Require(game.Place({3 + index * direction.x, 7 + index * direction.y}), "Black move legal");
            if (index < 4) Require(game.Place({index * 2, 14}), "White filler legal");
        }
        Require(game.State() == Status::Won && game.Winner() == Stone::Black &&
                game.WinningLine().size() == 5, "Five stones wins in all directions");
        Require(!game.Place({14, 0}), "Won board rejects moves");
    }
    game.Reset(); game.Start();
    for (int index = 0; index < 5; ++index) {
        Require(game.Place({2 * index, 14}), "Black filler legal");
        Require(game.Place({index, 0}), "White winning move legal");
    }
    Require(game.Winner() == Stone::White, "White can win too");
    // A three and a pair joined in the middle make six without an earlier five.
    game.Reset(); game.Start();
    for (int x : {0, 1, 2, 4, 5}) {
        Require(game.Place({x, 0}), "Overline setup legal");
        Require(game.Place({x * 2, 14}), "Overline filler legal");
    }
    Require(game.Place({3, 0}) && game.WinningLine().size() == 6, "Overlines win without forbidden moves");
    // This repeating coloring has no monochromatic five in any direction.
    game.Reset(); game.Start();
    std::vector<Move> black, white;
    for (int y = 0; y < rules::size; ++y) for (int x = 0; x < rules::size; ++x)
        (((x + 2 * y) % 4 < 2) ? black : white).push_back({x, y});
    Require(black.size() == white.size() + 1, "Draw fixture alternates legally");
    for (std::size_t index = 0; index < black.size(); ++index) {
        Require(game.Place(black[index]), "Draw black move legal");
        if (index < white.size()) Require(game.Place(white[index]), "Draw white move legal");
    }
    Require(game.State() == Status::Draw, "Full nonwinning board draws");
    game.Configure(Mode::Computer); game.Start();
    Require(game.HumanTurn(), "Human is Black first");
    game.Place({7, 7}); Require(!game.HumanTurn(), "Human cannot move during computer turn");
    game.Reset(); Require(game.PlayMode() == Mode::Computer && !game.LastMove(), "Reset retains mode and clears last move");
}
void TestSearch() {
    std::atomic<bool> cancelled{false};
    for (Strength strength : {Strength::Easy, Strength::Normal, Strength::Hard}) {
        Board board{};
        board[7 * rules::size + 2] = Stone::Black;
        for (int x = 3; x < 7; ++x) board[7 * rules::size + x] = Stone::White;
        const auto win = FindMove(board, Stone::White, OptionsFor(strength), cancelled);
        Require(win.move.x == 7 && win.move.y == 7, "AI takes immediate win at every difficulty");
        board.fill(Stone::Empty);
        board[7 * rules::size + 2] = Stone::White;
        for (int x = 3; x < 7; ++x) board[7 * rules::size + x] = Stone::Black;
        const auto block = FindMove(board, Stone::White, OptionsFor(strength), cancelled);
        Require(block.move.x == 7 && block.move.y == 7, "AI blocks immediate loss at every difficulty");
        board[2 * rules::size + 2] = Stone::Black;
        for (int x = 3; x < 7; ++x) board[2 * rules::size + x] = Stone::White;
        const auto priority = FindMove(board, Stone::White, OptionsFor(strength), cancelled);
        Require(priority.move.x == 7 && priority.move.y == 2, "Immediate win takes precedence over defense");
    }
    Board board{};
    const auto center = FindMove(board, Stone::Black, OptionsFor(Strength::Easy), cancelled);
    Require(center.move.x == 7 && center.move.y == 7, "Empty opening uses center");
    board[7 * rules::size + 7] = Stone::Black;
    const auto original = board;
    const auto limited = FindMove(board, Stone::White, {8, 14, 1, 1}, cancelled);
    Require(InBounds(limited.move.x, limited.move.y) && board[limited.move.y * rules::size + limited.move.x] == Stone::Empty && board == original,
            "Budget exhaustion returns legal fallback without changing snapshot");
    cancelled.store(true);
    Require(FindMove(board, Stone::White, OptionsFor(Strength::Hard), cancelled).move.x == -1, "Cancelled search returns no move");
    cancelled.store(false); board.fill(Stone::Black);
    Require(FindMove(board, Stone::White, OptionsFor(Strength::Easy), cancelled).move.x == -1, "Full board has no legal move");
}
void TestWorker() {
    GomokuGame game; game.Configure(Mode::Computer); game.Start(); game.Place({7, 7});
    GomokuAiTurn ai;
    ai.Update(game, Strength::Normal);
    Require(ai.Thinking(), "Search starts in background");
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(4);
    while (game.MoveCount() == 1 && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        ai.Update(game, Strength::Normal);
    }
    Require(game.MoveCount() == 2 && game.HumanTurn() && !ai.Thinking(), "Worker applies exactly one legal reply");
    game.Place({0, 0}); ai.Update(game, Strength::Hard);
    ai.Cancel(); game.Reset(); game.Start(); ai.Update(game, Strength::Hard);
    Require(game.MoveCount() == 0 && !ai.Thinking(), "Cancelled result cannot affect a restarted game");
    game.Place({7, 7}); ai.Update(game, Strength::Hard);
    game.Reset(); ai.Update(game, Strength::Hard);
    Require(!ai.Thinking() && game.MoveCount() == 0, "Changed state discards pending result");
}
}
int main() {
    try { TestRules(); TestSearch(); TestWorker(); std::cout << "Gomoku tests passed\n"; }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
